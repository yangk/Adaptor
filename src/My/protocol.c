#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "sdk.h"
#include "uart.h"
#include "flash.h"
#include "sdk_svc.h"
#include "comfunc.h"
#include "sec_tick.h"
#include "sec_printf.h"
#include "adaptor.h"
#include "macmap.h"
#include "protocol.h"
#include "update.h"
#include "protocol_gateway.h"

//---------------------------------------------------------------------------------------
struct SmartFrame *get_smart_frame(const uint8_t *in, int len)
{
    int i = 0;

 start_lbl:
    while (i < len)
    {
        if (STC == in[i])
            break;
        i++;
    }
    if (len - i < SMART_FRAME_HEAD)
        return NULL;
    struct SmartFrame *pframe = (struct SmartFrame *)&in[i];
    int dlen = pframe->len;

    if (i + SMART_FRAME_HEAD + dlen + 1 > len)
    {
        i++;
        goto start_lbl;
    }

    if (pframe->data[dlen] != checksum(pframe, dlen + SMART_FRAME_HEAD))
    {
        i++;
        goto start_lbl;
    }
    pframe = (struct SmartFrame *)&in[i];
    return pframe;
}
//---------------------------------------------------------------------------------------
int code_frame(const uint8_t *src, const uint8_t *dest, int seq, int cmd, 
    const uint8_t *data, int len, void *out, int maxlen)
{
    const uint8_t addr[AID_LEN] = {0x00, 0x00, 0x00, 0x00};

    static uint8_t _seq = 0;
    struct SmartFrame *pframe = (struct SmartFrame *)out;

    pframe->stc = STC;
    if (!src) src = addr;
    if (!dest) dest = addr;
    memcpy(pframe->said, src, AID_LEN);
    memcpy(pframe->taid, dest, AID_LEN);
    pframe->seq = seq < 0 ? (_seq++ & 0x7F) : seq;

    pframe->data[0] = cmd;
    memcpy(&pframe->data[1], data, len);
    pframe->len = len+1;
    pframe->data[pframe->len] = checksum(pframe, SMART_FRAME_HEAD + pframe->len);
    return SMART_FRAME_HEAD + pframe->len + 1;
}

int code_local_frame(const uint8_t *in, int len, void *out, int maxlen)
{
    return code_frame(NULL, NULL, 0, in[0], &in[1], len-1, out, maxlen);
}

int code_ret_frame(struct SmartFrame *pframe, int len)
{
    memcpy(pframe->taid, pframe->said, AID_LEN); 
    memcpy(pframe->said, adaptor.aid, AID_LEN); 
    pframe->seq |= 0x80;
    pframe->len = len;
    pframe->data[len] = checksum(pframe, pframe->len + SMART_FRAME_HEAD);
    return pframe->len + SMART_FRAME_HEAD + 1;
}

int code_body(uint16_t did, int err, const void *data, int len, void *out, int maxlen)
{
    struct Body *body = (struct Body *)out;

    put_le_val(did, body->did, sizeof(body->did));
    body->ctrl = get_bits(len, 0, 6);
    if (err) body->ctrl |= 0x80;
    memcpy(body->data, data, len);

    return sizeof(struct Body) + len;
}
//---------------------------------------------------------------------------------------
static estimer_t tmr_get_aid;
static void on_tmr_get_aid(estimer_t tmr, void *arg)
{
    uint8_t tmp[0x40];

    int len = code_frame(NULL, NULL, 0, CMD_REQ_AID, NULL, 0, tmp, sizeof(tmp));
    esuart_send_data(UART_CHN_1, tmp, len); 
}
void aid_init(void)
{
    tmr_get_aid = timer_create(200, 200, TMR_PERIODIC, on_tmr_get_aid, NULL, "tmr_get_aid");
    if (!tmr_get_aid)
    {
    	pr_info("creat tmr_get_aid failed!\n");
        return;
    }

    timer_start(tmr_get_aid);
}
static void check_aid_change(void)
{
    uint8_t last_aid[AID_LEN];
    const uint8_t broad_addr[AID_LEN] = {0xFF, 0xFF, 0xFF, 0xFF};

    memcpy(last_aid, (uint8_t*)FLASH_ADAPTOR_ADDR, sizeof(last_aid));
    if (!memcmp(last_aid, adaptor.aid, sizeof(last_aid))) 
        return;

    uint8_t body[0x100];
    int len = code_body(0x0604, 0, adaptor.aid, sizeof(adaptor.aid), body, sizeof(body));

    uint8_t tmp[0x100];
    len = code_frame(adaptor.aid, broad_addr, -1, CMD_SET, body, len, tmp, sizeof(tmp));

    do_plc_broadcast(SDK_KEY_INIT, tmp, len);
    flash_user_info_write(FLASH_ADAPTOR_ADDR, adaptor.aid, sizeof(adaptor.aid));
}
static int on_set_aid(uint8_t *data, int len)
{
    if (len != AID_LEN) return -1;

    timer_stop(tmr_get_aid, TMR_PERIODIC); 

    if (!memcmp(adaptor.aid, data, sizeof(adaptor.aid))) 
    {
    	data[0] = CMD_NAK;
        return 1;
    }

    memcpy(adaptor.aid, data, sizeof(adaptor.aid)); 
    check_aid_change();
    data[0] = CMD_ACK;
    return 1;
}
static int on_get_aid(uint8_t *data, int len)
{
    if (is_all_xx(adaptor.aid, 0, sizeof(adaptor.aid))) 
    {
        data[0] = CMD_NAK;
        return 1;
    }

    data[0] = CMD_ACK_AID;
    memcpy(&data[1], adaptor.aid, sizeof(adaptor.aid));
    return 1 + sizeof(adaptor.aid);
}
static int on_del_aid(uint8_t *data, int len)
{
    data[0] = CMD_ACK_AID;
    memset(adaptor.aid, 0, sizeof(adaptor.aid));

    timer_start(tmr_get_aid);
    return 1;
}
//---------------------------------------------------------------------------------------
static int on_get_eid(uint8_t *data, int len)
{
    sdk_svc_mac_t mac;
    sdk_err_t err = sdk_svc_get_mac(&mac);
    if (err != SDK_ERR_OK)
    {
    	pr_info("got svc mac failed, err[%d]!\n", err);
        return -1;
    }

    data[0] = CMD_ACK_EID;
    memcpy(&data[1], mac.mac, sizeof(mac.mac));
    memset(&data[1+sizeof(mac.mac)], 0, 2);
    return 9;
}
//---------------------------------------------------------------------------------------
static struct RegData reg_cache;
static int reg_cached = 0;
void ensure_reg(void)
{
    if (!reg_cached) return;
    memcpy(adaptor.panid, reg_cache.panid, sizeof(reg_cache.panid));
    memcpy(adaptor.gid, reg_cache.gid, sizeof(reg_cache.gid));
    memcpy(adaptor.sid, reg_cache.sid, sizeof(reg_cache.sid));
}
//---------------------------------------------------------------------------------------
static int on_get_sid(uint8_t *data, int len)
{
    ensure_reg();

    if (is_all_xx(adaptor.sid, 0, sizeof(adaptor.sid))) 
    {
        data[0] = CMD_NAK;
        return 1;
    }

    data[0] = CMD_ACK_SID;
    memcpy(&data[1], adaptor.sid, sizeof(adaptor.sid));
    return 1 + sizeof(adaptor.sid);
}
//---------------------------------------------------------------------------------------
static void bps_tmr_handle(estimer_t tmr, void *arg)
{
    esuart_config(UART_CHN_1, adaptor.baud, adaptor.parity); 
    timer_delete(tmr);
}
static int on_set_bps(uint8_t *data, int len)
{
    estimer_t bps_tmr;
    const uint32_t baud[] = {BAUD_2400, BAUD_4800, BAUD_9600, 57600, BAUD_115200};
    const uint32_t parity[] = {PARITY_NONE, PARITY_EVEN, PARITY_ODD};

    if (len != 2 || data[0] >= array_size(baud) || data[1] >= array_size(parity)) 
        return -1; 

    if (baud[data[0]] == adaptor.baud && parity[data[1]] == adaptor.parity) goto done;

    adaptor.baud = baud[data[0]];
    adaptor.parity = parity[data[1]];

    bps_tmr = timer_create(30, 0, TMR_TRIGGER, bps_tmr_handle, NULL, "bps_tmr");
    if (!bps_tmr)
    {
        pr_info("creat bps timer failed!\n");
        return -1;
    }

    timer_start(bps_tmr);
done:
    data[0] = CMD_ACK;
    return 1;
}
//---------------------------------------------------------------------------------------
static int on_set_reg(uint8_t *data, int len)
{
    data[0] = CMD_ACK;
    adaptor.regtype = data[0] & 0x01;
    return 1;
}
//---------------------------------------------------------------------------------------
static void do_notify_link(uint8_t *dst, int unlink)
{
    uint8_t reg[AID_LEN+SID_LEN];
    memcpy(&reg[0], adaptor.aid, sizeof(adaptor.aid));
    memcpy(&reg[AID_LEN], adaptor.sid, sizeof(adaptor.sid)); 

    uint8_t body[0x100];
    int len = code_body(0x0603, 0, reg, sizeof(reg), body, sizeof(body));
    if (unlink) ((struct Body *)body)->ctrl |= 0x80;

    uint8_t tmp[0x100];
    len = code_frame(adaptor.aid, adaptor.gid, -1, CMD_SET, body, len, tmp, sizeof(tmp));

    do_plc_unicast(dst, SDK_KEY_INIT, tmp, len);
    if (unlink) del_mac_by_id(adaptor.gid);
}

static int on_set_unlink(uint8_t *data, int len)
{
    uint8_t dst[6];

    ensure_reg();
    if (!is_all_xx(adaptor.gid, 0, AID_LEN) && get_mac_by_id(adaptor.gid, dst) >= 0) 
    {
        do_notify_link(dst, 1);
    }

    data[0] = CMD_ACK;
    return 1;
}
//---------------------------------------------------------------------------------------
static int on_get_ver(uint8_t *data, int len)
{
    const char *soft_ver = get_dev_ver();

    data[0] = CMD_ACK_VER;

    strcpy((char *)&data[1], soft_ver); 
    return 1 + strlen(soft_ver);
}
//---------------------------------------------------------------------------------------
static int on_set_panid(uint8_t *data, int len)
{
    if (len != SID_LEN) return -1;

    memcpy(adaptor.panid, data, sizeof(adaptor.panid));
    do_set_psk();

    data[0] = CMD_ACK;
    return 1;
}
static int on_get_panid(uint8_t *data, int len)
{
    if (is_all_xx(adaptor.panid, 0, sizeof(adaptor.panid))) 
    {
        data[0] = CMD_NAK;
        return 1;
    }

    data[0] = CMD_ACK_PANID;
    memcpy(&data[1], adaptor.panid, sizeof(adaptor.panid));

    return 1 + sizeof(adaptor.panid);
}
//---------------------------------------------------------------------------------------
static int on_get_gid(uint8_t *data, int len)
{
    uint8_t dst[6];

    if (!is_all_xx(adaptor.gid, 0, AID_LEN) 
        && memcmp(adaptor.gid, reg_cache.gid, AID_LEN)
        && get_mac_by_id(adaptor.gid, dst) >= 0) 
    {
        do_notify_link(dst, 1);
    }
    ensure_reg();

    if (is_all_xx(adaptor.gid, 0, sizeof(adaptor.gid))) 
    {
        data[0] = CMD_NAK;
        return 1;
    }

    if (!is_all_xx(adaptor.gid, 0, sizeof(adaptor.gid)) 
        && get_mac_by_id(adaptor.gid, dst) >= 0) 
    {
        do_notify_link(dst, 0);
        set_id_attr(adaptor.gid, MAC_ATTR_TMLESS);
        do_set_psk();
    }

    data[0] = CMD_GET_GWAID;
    memcpy(&data[1], adaptor.gid, sizeof(adaptor.gid));
    return 1 + sizeof(adaptor.gid);
}

//---------------------------------------------------------------------------------------
static int on_tst_plc(uint8_t *data, int len)
{
    int ret = 0;
    void *retd = NULL;
    uint8_t tmp[0x40];

    if (len < 1) return -1;

    uint8_t tst_cmd = data[0];
    uint8_t *tst_data=&data[1];
    len--;
    switch (tst_cmd)
    {
    case SDK_TST_SFO:
        {
            sdk_svc_tst_sfo_t *sfo = (sdk_svc_tst_sfo_t *)tmp;
            if (len < sizeof(sfo->dst)) return -1;
            memcpy(sfo->dst, tst_data, sizeof(sfo->dst));
        }
        break;
    case SDK_TST_GET_SFO:
        {
            sdk_svc_tst_get_sfo_t *sfo = (sdk_svc_tst_get_sfo_t *)tmp;
            if (len < sizeof(sfo->dst)) return -1;
            memcpy(sfo->dst, tst_data, sizeof(sfo->dst));
            retd = &sfo->sfo;
            ret = sizeof(sfo->sfo);
        }
        break;
    case SDK_TST_REQ:
        {
            int idx = 0;
            sdk_svc_tst_req_t *req = (sdk_svc_tst_req_t *)tmp;
            if (len < sizeof(sdk_svc_tst_req_t)) return -1;
            memcpy(req->dst, &tst_data[idx], sizeof(req->dst));
            idx += sizeof(req->dst);
            req->tmi.mm   = tst_data[idx++];
            req->tmi.copy = tst_data[idx++];
            req->tmi.fec  = tst_data[idx++];
            req->frm.pb_amt =tst_data[idx++];
            req->frm.pb_type=tst_data[idx++];
        }
        break;
    case SDK_TST_GAIN:
        {
            sdk_svc_tst_gain_t *gain = (sdk_svc_tst_gain_t *)tmp;
            retd = gain;
            ret = sizeof(*gain);
        }
        break;
    case SDK_TST_GETNB:
        {
            retd = tmp;
            ret = sizeof(sdk_svc_mac_t);
        }
        break;
    case SDK_TST_CLKNB:
        break;
    default:
        return -1;
    }

    sdk_err_t err = sdk_svc_tst(tst_cmd, tmp);
    if (err != SDK_ERR_OK)
    {
        pr_info("tst plc function failed, err[%d]!\n", err);
        data[0] = CMD_NAK;
        return 1;
    }

    data[0] = CMD_TST_PLC;
    data[1] = tst_cmd;
    memcpy(&data[2], retd, ret);

    return 2 + ret;
}
void do_evt_tst_resp(const sdk_evt_tst_resp_t *resp)
{
    uint8_t tmp[0x100];

    int idx = 0;
    uint8_t data[0x40];
    data[idx++] = CMD_TST_PLC;
    data[idx++] = SDK_TST_REQ;
    memcpy(&data[idx], resp->src, sizeof(resp->src));
    idx += sizeof(resp->src);
    data[idx++] = resp->tmi.mm;
    data[idx++] = resp->tmi.copy;
    data[idx++] = resp->tmi.fec;
    data[idx++] = resp->frm.pb_amt;
    data[idx++] = resp->frm.pb_type;
    data[idx++] = resp->rgain;
    idx = code_local_frame(data, idx, tmp, 0x100);
    esuart_send_data(UART_CHN_1, tmp, idx);
}
//---------------------------------------------------------------------------------------
static int on_chg_tone(uint8_t *data, int len)
{
    if (len != 5) return -1;

    uint8_t tone = data[0];
    if (tone != SDK_TONE_131 && tone != SDK_TONE_411)
        return -1;

    int tgain = get_le_val(&data[1], 4);
    sdk_err_t err = sdk_svc_set_tone(tone, tgain);
    if (err != SDK_ERR_OK)
    {
        pr_info("sdk_svc_set_tone failed, err[%d]!\n", err);
        data[0] = CMD_NAK;
        return 1;
    }

    data[0] = CMD_ACK;
    return 1;
}
//---------------------------------------------------------------------------------------
static const struct LocalOps ops[] = 
{
    {"SET AID", CMD_SET_AID, on_set_aid},
    {"GET AID", CMD_GET_AID, on_get_aid},
    {"DEL AID", CMD_DEL_AID, on_del_aid},
    {"GET SID", CMD_GET_SID, on_get_sid},
    {"GET EID", CMD_GET_EID, on_get_eid},
    {"SET BPS", CMD_SET_BPS, on_set_bps},
    {"SET REG", CMD_SET_REG, on_set_reg},
    {"UNLINK",  CMD_UNLINK,  on_set_unlink},
    {"SET PID", CMD_SET_PANID, on_set_panid},
    {"GET GID", CMD_GET_GWAID, on_get_gid},
    {"GET VER", CMD_GET_VER,   on_get_ver},
    {"GET PID", CMD_GET_PANID, on_get_panid},
    {"TST PLC", CMD_TST_PLC,  on_tst_plc},
    {"CHG TONE",CMD_CHG_TONE, on_chg_tone},
};
static const struct LocalOps *get_ops(int cmd)
{
    int i;

    for (i = 0; i < array_size(ops); ++i)
    {
        if (ops[i].cmd == cmd) 
        {
            return &ops[i];
        }
    }
    return NULL;
}
static int do_local_frame(struct SmartFrame *pframe)
{
    const struct LocalOps *ops = get_ops(pframe->data[0]);
    if (!ops)
    {
        pr_info("not find cmd, cmd[%d]!\n", pframe->data[0]);
        return -1;
    }

    pr_info("Local Frame[%s]!\n", ops->name); 
    int ret = ops->handle(&pframe->data[1], pframe->len-1);
    if (ret > 0)
    {
        ret = code_local_frame(&pframe->data[1], ret, pframe, 0x100);
        esuart_send_data(UART_CHN_1, (uint8_t*)pframe, ret);
    }
    return 0;
}
//---------------------------------------------------------------------------------------
int is_my_smart_frame(const struct SmartFrame *frame)
{
    if (is_all_xx(adaptor.aid, 0x00, AID_LEN))
    {
        pr_info("I do not have aid, drop it!\n");
        return 0;
    }

    if (!is_all_xx(frame->taid, 0xFF, AID_LEN) && memcmp(frame->taid, adaptor.aid, AID_LEN)) 
    {
        return 0;
    }
    return 1;
}
//---------------------------------------------------------------------------------------
static int is_lowpw_frame(struct SmartFrame *pframe)
{
    int i;
    const uint16_t low_did[] = {0x0004, 0x0603, 0x0604, 0x0605, 0x0606, 0x0607, 0x0608, 0x060A};

    struct AppFrame *app = (struct AppFrame *)pframe->data;
    uint16_t did = get_le_val(((struct Body *)app->data)->did, DID_LEN);

    for (i = 0; i < array_size(low_did); i++)
    {
        if (did == low_did[i])
            return 1;
    }
    return 0;
}

int do_send_frame(struct SmartFrame *frame)
{
    uint8_t dst[6];

    int key_mod = is_lowpw_frame(frame) ? SDK_KEY_INIT : SDK_KEY_USER;
    if (is_all_xx(frame->taid, 0xFF, AID_LEN) || get_mac_by_id(frame->taid, dst) < 0) 
    {
        return do_plc_broadcast(key_mod, frame, frame_len(frame));
    }
    return do_plc_unicast(dst, key_mod, frame, frame_len(frame));
}
//---------------------------------------------------------------------------------------
static void reboot_tmr_handle(estimer_t tmr, void *arg)
{
    sdk_svc_reset();
}
static int do_reboot(const uint8_t *src, struct Body *body)
{
    uint32_t wait = 2000; //default 2s
    if (get_bits(body->ctrl, 0, 6) == 2) 
        wait = get_le_val(body->data, 2); 

    estimer_t reboot_tmr = timer_create(wait/10, 0, TMR_TRIGGER, 
        reboot_tmr_handle, NULL, "reboot_tmr");
    if (!reboot_tmr)
    {
        pr_info("creat bps timer failed!\n");
        return -OTHER_ERR;
    }

    timer_start(reboot_tmr);
    return NO_ERR;
}
static int do_get_ver(const uint8_t *src, struct Body *body)
{
    const char *soft_ver = get_dev_ver();
    strcpy((char *)body->data, soft_ver); 
    return strlen(soft_ver);
}
static int do_get_ext_ver(const uint8_t *src, struct Body *body)
{
    strcpy((char *)body->data, adaptor_ext_ver); 
    reverse(body->data, strlen(adaptor_ext_ver));
    return strlen(adaptor_ext_ver);
}
static int do_get_plc_ver(const uint8_t *src, struct Body *body)
{
    const char *soft_ver = get_plc_ver();
    strcpy((char *)body->data, soft_ver); 
    return strlen(soft_ver);
}
static int do_get_dev_type(const uint8_t *src, struct Body *body)
{
    strcpy((char *)body->data, adaptor_soft_type); 
    return strlen(adaptor_soft_type);
}
static int do_get_plc_type(const uint8_t *src, struct Body *body)
{
    strcpy((char *)body->data, adaptor_plc_type); 
    return strlen(adaptor_plc_type);
}
static int do_reg(const uint8_t *src, struct Body *body)
{
    if (get_bits(body->ctrl, 0, 6) != sizeof(struct RegData)) 
        return -LEN_ERR; 

    struct RegData *reg = (struct RegData *)body->data; 
    if (is_all_xx(adaptor.aid, 0x00, AID_LEN) || memcmp(adaptor.aid, reg->aid, AID_LEN))
        return -OTHER_ERR;

    memcpy(&reg_cache, reg, sizeof(reg_cache));
    reg_cached = 1;

    uint8_t tmp[0x40];
    int len = code_frame(NULL, NULL, 0, CMD_REGINFOR, reg->panid,
        sizeof(reg->panid) + sizeof(reg->pw), tmp, sizeof(tmp));
    esuart_send_data(UART_CHN_1, tmp, len);
    return -NO_RETURN;
}
static int do_notify_aid(const uint8_t *src, struct Body *body)
{
    if (get_bits(body->ctrl, 0, 6) == 4) 
        add_mac_by_id(body->data, src, NULL, 0);
    return -NO_RETURN;
}
static int do_get_aid(const uint8_t *src, struct Body *body)
{
    if (is_all_xx(adaptor.aid, 0, AID_LEN) || memcmp(body->data, adaptor.aid, AID_LEN)) 
        return -NO_RETURN;

    memcpy(body->data, adaptor.aid, AID_LEN);
    return AID_LEN;
}
//---------------------------------------------------------------------------------------
enum
{
    HCTR_LOWPW = 1 << 0,
};

struct func_ops
{
    int did;
    const char *tip;
    uint32_t ctrl;
    int (*read)  (const uint8_t *src, struct Body *body);
    int (*write) (const uint8_t *src, struct Body *body);
    int (*read_resp)  (const uint8_t *src, struct Body *body);
    int (*write_resp) (const uint8_t *src, struct Body *body);
};
const struct func_ops func_items[] = 
{
    {0x0004, "GetExtVer",  0, do_get_ext_ver, NULL, NULL, NULL},
    {0x0601, "Reboot",     0, NULL, do_reboot, NULL, NULL},
    {0x0602, "Get DevType",0, do_get_dev_type, NULL, NULL, NULL},
    {0x0603, "Register",   0, NULL, do_reg, NULL, NULL},
    {0x0604, "Notify Aid", 0, NULL, do_notify_aid, NULL, NULL},
    {0x0605, "Get Aid",    0, do_get_aid, NULL, NULL, NULL},
    {0x0606, "GetIntVer",  0, do_get_ver, NULL, NULL, NULL},
    {0x0607, "GpidInfo",   0, NULL, NULL, NULL, NULL},
    {0x0608, "SetHiepsk",  0, NULL, NULL, NULL, NULL},
    {0x0609, "GetPLCType", 0, do_get_plc_type, NULL, NULL, NULL},
    {0x060A, "GetPLCVer",  0, do_get_plc_ver, NULL, NULL, NULL},
};
const struct func_ops func_items_lbs[] = 
{
    {0x0004, "GetExtVer",  0, do_get_ext_ver, NULL, NULL, NULL},
    {0x0601, "Reboot",     0, NULL, do_reboot, NULL, NULL},
    {0x0602, "Get DevType",0, do_get_dev_type, NULL, NULL, NULL},
    {0x0603, "Register",   0, NULL, register_result, NULL, NULL},
    {0x0604, "Notify Aid", 0, NULL, do_notify_aid, NULL, NULL},
    {0x0606, "GetIntVer",  0, do_get_ver, NULL, NULL, NULL},
    {0x0607, "GpidInfo",   0, NULL, NULL, NULL, NULL},
    {0x0608, "SetHiepsk",  0, NULL, NULL, NULL, NULL},
    {0x0609, "GetPLCType", 0, do_get_plc_type, NULL, NULL, NULL},
    {0x060A, "GetPLCVer",  0, do_get_plc_ver, NULL, NULL, NULL},
};

static const struct func_ops *get_option(int did)
{
    int i;

    const struct func_ops *funcs = adaptor.type == APP_TYPE_LBS ? func_items_lbs : func_items;
    int funcs_nr = adaptor.type == APP_TYPE_LBS ? array_size(func_items_lbs) : array_size(func_items);

    for (i = 0; i < funcs_nr; i++)
    {
        if (funcs[i].did == did)
            return &funcs[i];
    }
    return NULL;
}
//---------------------------------------------------------------------------------------
static int form_error_body(void *out, uint8_t * did, int err)
{
    struct Body *body = (struct Body *)out;

    memcpy(body->did, did, sizeof(body->did));
    body->ctrl = 0x82;
    put_le_val(err, body->data, 2);

    return offsetof(struct Body, data)+get_bits(body->ctrl, 0, 6);
}
//---------------------------------------------------------------------------------------
static int do_cmd(const sdk_evt_rx_plc_t *info, int cmd, int resp, uint8_t *data, int len)
{
    int outidx = 0, inidx = 0;

    uint8_t tmp[BUF_LEN];
    memcpy(tmp, data, len);

    while (len >= FBD_FRAME_HEAD)
    {
        struct Body *body = (struct Body *)&tmp[inidx];
        struct Body *outbd = (struct Body *)&data[outidx];

        int dlen = get_bits(body->ctrl, 0, 6);
        if (len < FBD_FRAME_HEAD + dlen)
        {
            pr_info("Smart frame, body length error!\n");
            outidx += form_error_body(outbd, body->did, LEN_ERR);
            break;
        }
        inidx += FBD_FRAME_HEAD + dlen;
        len -= FBD_FRAME_HEAD + dlen;

        const struct func_ops *op = get_option(get_le_val(body->did, sizeof(body->did)));
        int (*handler)(const uint8_t *src, struct Body *body) = NULL;
        if (cmd == CMD_READ)
        {
            handler = op ? (resp ? op->read_resp : op->read) : NULL;
        }
        else
        {
            handler = op ? (resp ? op->write_resp : op->write) : NULL;
        }

        if (handler)
        {
            pr_info("Do cmd[%s]...\n", op->tip);

            memcpy(outbd, body, sizeof(struct Body) + dlen);
            int ret = handler(info->src, outbd); 
            if (ret < 0)
            {
                if (ret == -NO_RETURN) continue;
                form_error_body(outbd, body->did, -ret);
            }
            else if (ret > 0)
            {
                memcpy(outbd->did, body->did, sizeof(body->did));
                outbd->ctrl = ret;
            }
            else
            {
                body->ctrl = 0;
            }
        }
        else 
        {
            return -1;
        }
        outidx += FBD_FRAME_HEAD + get_bits(outbd->ctrl, 0, 6);
    }
    return outidx;
}
//---------------------------------------------------------------------------------------
int resend_check(const uint8_t *src, const struct SmartFrame *pframe)
{
    static uint8_t last_seq, last_src[6], last_sum;
    static uint32_t last_tm;

    uint32_t cur_seq = get_bits(pframe->seq, 0, 6);
    uint32_t cur_tm = sec_jiffies; 
    uint8_t cur_sum = pframe->data[pframe->len];
    if (!memcmp(src, last_src, sizeof(last_src)) 
        && cur_seq == last_seq 
        && cur_sum == last_sum
        && time_after(last_tm+SEC_HZ/5, cur_tm)) 
    {
        pr_info("Receivd dummy frame, drop it!, "
                              "last_tm[%d], cur_tm[%d]\n", last_tm, cur_tm);
        return -1;
    }
    memcpy(last_src, src, sizeof(last_src));
    last_seq = cur_seq;
    last_sum = cur_sum;
    last_tm = cur_tm;
    return 0;
}
//---------------------------------------------------------------------------------------
/* 
 data[0]: bit[0~5]: data length
          bit[6~7]: group type
 group_type: 0: bit
             1: 1byte  group id
             2: 2bytes group id
*/
static int is_gid_equal(const uint8_t *data)
{
    int dlen = get_bits(data[0], 0, 5);
    int group_type = get_bits(data[0], 6, 7);

    data++;
    if (group_type != 0) /* bytes type */
    {
        int i;
        int gid_unit_len = group_type == 1 ? 1 : 2;

        for (i = 0; i < dlen; i += gid_unit_len) 
        {
            int _gid = get_le_val(data + i, gid_unit_len);
            if (_gid == 0) return 1;
        }
    }

    return 0;
}

int do_group_cmd(const sdk_evt_rx_plc_t *info, int resp, uint8_t *data, int len)
{
    int inidx = 0, gid_len;

    gid_len = get_bits(data[inidx], 0, 5) + 1;
    while (len >= FBD_FRAME_HEAD + gid_len)
    {
        struct Body *body = (struct Body *)&data[inidx + gid_len];
        int body_len = gid_len + FBD_FRAME_HEAD + get_bits(body->ctrl, 0, 6);

        if (len < body_len)
            break;

        if (is_gid_equal(&data[inidx]))
        {
            const struct func_ops *op = get_option(get_le_val(body->did, sizeof(body->did)));
            int (*handler)(const uint8_t *src, struct Body *body) = NULL;
            handler = op ? (resp ? op->write_resp : op->write) : NULL;
            if (handler)
            {
                pr_info("Do cmd[%s]...\n", op->tip);
                handler(info->src, body);
                return 0;
            }
        }

        inidx += body_len;
        len -= body_len;

        gid_len = get_bits(data[inidx], 0, 5) + 1;
    }
    return -1;
}
//---------------------------------------------------------------------------------------
static int plc_frame_hook(const sdk_evt_rx_plc_t *info, struct SmartFrame *pframe)
{
    int ret = -1;
    struct AppFrame *app = (struct AppFrame *)pframe->data;

    int len = pframe->len - sizeof(struct AppFrame);
    switch (app->cmd) 
    {
    case CMD_READ:
    case CMD_SET:
        if (is_all_xx(pframe->taid, 0xff, AID_LEN) && app->cmd == CMD_SET)
            ret = do_group_cmd(info, tst_bit(pframe->seq, 7), app->data, len);
        else
            ret = do_cmd(info, app->cmd, tst_bit(pframe->seq, 7), app->data, len); 
        break;
    case CMD_UPDATE:
        {
            if (tst_bit(pframe->seq, 7)) 
                return -1;
            ret = do_update(app->data, len);
        }
        break;
    default:
        break;
    }
    if (ret > 0 && !is_all_xx(pframe->taid, 0xFF, sizeof(pframe->taid))) 
    {
        ret += sizeof(struct AppFrame);
        code_ret_frame(pframe, ret);
        return do_send_frame(pframe);
    }
    return ret;
}

int do_plc_frame(const sdk_evt_rx_plc_t *info, struct SmartFrame *pframe)
{
    if (info->key_mod == SDK_KEY_INIT && !is_lowpw_frame(pframe))
    {
        pr_info("Error: this frame need strong password\n");
        return 0;
    }

    if (plc_frame_hook(info, pframe) < 0)
    {
    	if(adaptor.type == APP_TYPE_LBS)
    		recv_node_info(info->src, pframe);
    	else
    		esuart_send_data(UART_CHN_1, (uint8_t *)pframe, frame_len(pframe));
    }
    return 0;
}
//---------------------------------------------------------------------------------------
int smart_frame_handle(struct SmartFrame *frame)
{
    if (is_all_xx(frame->said, 0x00, AID_LEN) && is_all_xx(frame->taid, 0x00, AID_LEN))
    {
        return do_local_frame(frame);
    }
    else if (!memcmp(frame->said, adaptor.aid, AID_LEN) && !is_all_xx(frame->taid, 0x00, AID_LEN)) 
    {
        return do_send_frame(frame);
    }
    else 
    {
        pr_info("invalid frame, drop it!\n");
    }
    return 0;
}
//---------------------------------------------------------------------------------------
