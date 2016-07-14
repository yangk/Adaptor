#include "include.h"
#include "sdk_svc.h"
#include "sdk_evt.h"
#include "timer.h"
#include "gpio.h"
#include "uart.h"
#include "comfunc.h"
#include "sec_printf.h"
#include "cmd.h"
#include "sec_tick.h"
#include "macmap.h"
#include "update.h"
#include "protocol.h"
#include "protocol_gateway.h"
#include "adaptor.h"
#include "lightelf_gpio_reg.h"

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
const char *adaptor_ext_ver = ADAPTOR_VERSION_EXT;
const char *adaptor_soft_type = ADAPTOR_MATERIAL_CODE;
const char *adaptor_plc_type = PLC_MATERIAL_CODE;

const uint8_t plc_dev_type[8] = {0xFF, 0xFF, 0x32, 0x00, 0x00, 0x00, 0x00, 0x01};
const uint8_t adaptor_dev_type[8] = {0xFF, 0xFF, 0x32, 0x00, 0x00, 0x00, 0x00, 0x02};
//-------------------------------------------------------------------------------------------------
struct Adaptor adaptor;
//-------------------------------------------------------------------------------------------------
int get_dev_type(uint8_t *type)
{
    memcpy(type, adaptor_dev_type, sizeof(adaptor_dev_type));
    return sizeof(adaptor_dev_type);
}
int get_plc_type(uint8_t *type)
{
    memcpy(type, plc_dev_type, sizeof(plc_dev_type));
    return sizeof(plc_dev_type);
}
int get_dev_type_by_layer(enum LAYER_TYPE layer, uint8_t *dev_type)
{
    switch (layer) 
    {
    case LAYER_PLC:
        return get_plc_type(dev_type);
    case LAYER_ADAPTOR:
        return get_dev_type(dev_type);
    default:
        return -1;
    }
}
enum LAYER_TYPE get_layer_by_dev_type(const uint8_t *dev_type) 
{
    if (!memcmp(adaptor_dev_type, dev_type, sizeof(adaptor_dev_type))) 
    {
        return LAYER_ADAPTOR;
    }
    else if (!memcmp(plc_dev_type, dev_type, sizeof(plc_dev_type))) 
    {
        return LAYER_PLC;
    }
    return LAYER_NR;
}
//-------------------------------------------------------------------------------------------------
const char *get_dev_ver(void)
{
    static char dev_ver[0x40];

    snprintf(dev_ver, sizeof(dev_ver), "ESMD-AD63(v%01x.%02x)-20%06x", 
             get_bits(ADAPTOR_VERSION, 8, 15), get_bits(ADAPTOR_VERSION, 0, 7), 
             get_bits(ADAPTOR_DATE, 0, 23));
    return dev_ver;
}
const char *get_plc_ver(void)
{
    sdk_err_t err;
    sdk_svc_ver_t ver;
    static char plc_ver[0x40];

    if ((err = sdk_svc_get_ver(&ver)) != SDK_ERR_OK) 
    {
        pr_err("sdk_svc_get_ver failed, err[%d]\n", err);
        return NULL;
    }

    snprintf(plc_ver, sizeof(plc_ver), "SSC1663-PLC(v%01x.%02x)-20%06x", 
             get_bits(ver.ver, 8, 15), get_bits(ver.ver, 0, 7), 
             get_bits(ver.date, 0, 23));
    return plc_ver;
}
const char *get_soft_ver_by_layer(enum LAYER_TYPE layer)
{
    switch (layer) 
    {
    case LAYER_PLC:
        return get_plc_ver();
    case LAYER_ADAPTOR:
        return get_dev_ver();
    default:
        return NULL;
    }
}
//-------------------------------------------------------------------------------------------------
void adaptor_show(void)
{
    sdk_svc_mac_t mac;
    sdk_svc_get_mac(&mac);
    memcpy(adaptor.mac, mac.mac, MAC_LEN);
    esprintf("MAC:[%02X:%02X:%02X:%02X:%02X:%02X]\n",
        mac.mac[0], mac.mac[1], mac.mac[2], mac.mac[3], mac.mac[4], mac.mac[5]);

    sdk_svc_ver_t plc;
    sdk_svc_get_ver(&plc);
    esprintf("PLC:[Manu:%04X,Chn:%02X,Mod:%02X,Config:%04X,Ver:%04X,Data:%08X]\n",
        plc.manu, plc.chl, plc.mod, plc.config, plc.ver, plc.date);

    esprintf("App:[Type:%02X,Ver:%s]\n", adaptor.type, get_dev_ver()); 

    esprintf("AID:[%02X %02X %02X %02X], PANID:[%02X %02X]\n",
        adaptor.aid[0], adaptor.aid[1], adaptor.aid[2], adaptor.aid[3], adaptor.panid[0], adaptor.panid[1]);
}
//-------------------------------------------------------------------------------------------------
static void mod_check(void)
{
    uint32_t mode = gpio_input(GPIO_12);
    switch (mode)
    {
    case 1:
        adaptor.type = APP_TYPE_LBS;
        adaptor.baud = BAUD_115200;
        break;
    case 0:
    default:
        adaptor.type = APP_TYPE_NORMAL;
        adaptor.baud = BAUD_9600;
        break;
    }
    adaptor.parity = PARITY_NONE;
    esuart_config(UART_CHN_1, adaptor.baud, adaptor.parity);
}
//-------------------------------------------------------------------------------------------------
static void app_init(void)
{
    switch (adaptor.type)
    {
    case APP_TYPE_LBS:
        notice_plc_chip_reset();
        break;
    case APP_TYPE_NORMAL:
    default:
        aid_init();
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void adaptor_init(void)
{
	SET_GPIO_GPIO_MDIO_EN(0);				/* 设置gpio22为普通gpio功能 */
	SET_GPIO_GPIO_JTAG_EN(0);
	gpio_out_high(TX_LED);
    memset(&adaptor, 0, sizeof(adaptor));

    sec_cmd_init();
    esuart_init();
    mod_check();
    adaptor_show();
    app_init();
    update_init();
    sec_tick_init();
}
//-------------------------------------------------------------------------------------------------
int do_plc_broadcast(int key_mod, const void *data, int len)
{
    sdk_svc_brd_t info;

    pr_info("PLC[Broadcast]:\n");
    print_debug_array(KERN_INFO, data, len);

    info.src_mod = SDK_MOD_6364;
    info.dst_mod = SDK_MOD_6364;
    info.prio = SDK_PRIO_LOW;
    info.key_mod = key_mod;
    info.radius = 0; /* max radius */
    info.multi = SDK_MULTI_DIS;
    if (adaptor.type == APP_TYPE_NORMAL 
        && is_all_xx(adaptor.panid, 0x00, sizeof(adaptor.panid))) 
    {
        info.radius = 1;
    }
    sdk_err_t err = sdk_svc_brodcast(&info, (uint8_t *)data, len);
    if (err != SDK_ERR_OK)
    {
    	pr_info("sdk_svc_brodcast failed, err[%d]!\n", err);
    }
    
    return 0;
}
int do_plc_unicast(const uint8_t *dst, int key_mod, const void *data, int len)
{
    sdk_svc_uni_t info;

	char str[0x40];
	pr_info("PLC[Unicast to %s]:\n", arr2str(dst, 6, str, sizeof(str)));
    print_debug_array(KERN_INFO, data, len);

    info.src_mod = SDK_MOD_6364;
    info.dst_mod = SDK_MOD_6364;
    info.prio = SDK_PRIO_LOW;
    info.key_mod = key_mod;
    memcpy(info.dst, dst, sizeof(info.dst));
    info.multi = SDK_MULTI_DIS;
    
    sdk_err_t err = sdk_svc_unicast(&info, (uint8_t *)data, len);
    if (err != SDK_ERR_OK)
    {
        pr_info("sdk_svc_unicast failed, err[%d]!\n", err);
        return -1;
    }
    return 0;
}
int do_set_psk(void)
{
    sdk_err_t err;
    sdk_svc_psk_t info;

    memset(&info, 0, sizeof(info));
    info.len = 8;
    if (is_all_xx(adaptor.panid, 0, sizeof(adaptor.panid))) 
    {
        strcpy((char*)info.psk, "eastsoft");
    }
    else
    {
        memcpy(info.psk, adaptor.panid, sizeof(adaptor.panid));
    }

    err = sdk_svc_set_psk(&info);
    if (err != SDK_ERR_OK)
    {
		pr_info("got svc version failed, err[%d]!\n", err);
        return -1;
    }
    return 0;
}
int do_psk_resp(const uint8_t *dst, int state)
{
    sdk_svc_rmtpsk_resp_t info;

    info.src_mod = SDK_MOD_6364;
    info.dst_mod = SDK_MOD_6364;
    info.prio = SDK_PRIO_LOW;
    memcpy(info.dst, dst, sizeof(info.dst));
    info.state = state;
    info.multi = SDK_MULTI_DIS;

    /* deny networking */
    sdk_err_t err = sdk_svc_rmtpsk_resp(&info);
    if (err != SDK_ERR_OK)
    {
		pr_info("sdk_svc_rmtpsk_resp failed, err[%d]!\n", err);
        return -1;
    }
    pr_info("sdk_svc_rmtpsk_resp, state[%d]!\n", state);
    return 0;
}
//-------------------------------------------------------------------------------------------------
int adaptor_on_uart1_received(const uint8_t *data, int len)
{
    struct LbsFrame *pLbs = get_lbs_frame(data, len);
    if (pLbs)
    {
        lbs_frame_handle(pLbs);
        return len;
    }

    struct SmartFrame *frame = get_smart_frame(data, len);
    if (frame)
    {
        len = ((const uint8_t *)frame - data) + frame_len(frame);
        smart_frame_handle(frame);
        return len;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
static int do_devprop_req(const uint8_t *src, int taskid, int state,
    const void *data, int len)
{
    sdk_err_t err;
    sdk_svc_devprop_resp_t info;

    info.src_mod = SDK_MOD_6364;
    info.dst_mod = SDK_MOD_6364;
    info.state = state;
    memcpy(info.src, src, sizeof(info.src));
    info.taskid = taskid;
    info.multi = SDK_MULTI_DIS;

    err = sdk_svc_devprop_resp(&info, (uint8_t *)data, len);
    if (err != SDK_ERR_OK)
    {
		pr_info("got svc version failed, err[%d]!\n", err);
        return -1;
    }
    pr_info("respond devprop frame[%s], att:\n", state == SDK_SCH_PARTICIPATE ? "PART" : "ABS");
    print_debug_array(KERN_INFO, data, len);
    return 0;
}
//-------------------------------------------------------------------------------------------------
void adaptor_on_evt_devprop_req(const sdk_evt_devprop_req_t *req, const uint8_t *att, int len)
{
	pr_info("got devprop frame, att:\n");
    print_debug_array(KERN_INFO, att, len);

    int state = SDK_SCH_ABSENT;

    switch (len)
    {
    case 0:
        state = SDK_SCH_PARTICIPATE;
        break;
    case SID_LEN:   // att: PANID. Gateway first poweron
        if (adaptor.type == APP_TYPE_NORMAL && !memcmp(att, adaptor.panid, sizeof(adaptor.panid)))
            state = SDK_SCH_PARTICIPATE;
        break;
    case AID_LEN:
        if (!is_all_xx(adaptor.aid, 0x00, AID_LEN) && !memcmp(att, adaptor.aid, AID_LEN))
            state = SDK_SCH_PARTICIPATE;
        break;
    default:
        break;
    }
    do_devprop_req(req->src, req->taskid, state, adaptor.aid, sizeof(adaptor.aid));
}
//-------------------------------------------------------------------------------------------------
void adaptor_on_evt_rx_plc(const sdk_evt_rx_plc_t *info, const uint8_t *data, int len)
{
	char str[0x40];
	pr_info("PLC[RX], mac[%s]:\n",
        arr2str(info->src, sizeof(info->src), str, sizeof(str)));
    print_debug_array(KERN_INFO, data, len);

    struct SmartFrame *frame = get_smart_frame(data, len);
    if (frame && is_my_smart_frame(frame))
    {
        if (resend_check(info->src, frame) < 0)
            return;
        if(adaptor.type == APP_TYPE_NORMAL)
        {
        	add_mac_by_id(frame->said, info->src, 0, 0);
        }
        do_plc_frame(info, frame);
    }
}
//-------------------------------------------------------------------------------------------------
void adaptor_evt_sch_rpt(const sdk_evt_sch_rpt_t *resp)
{
    if (resp == NULL)
        return;
    int i;
    for (i = 0; i < resp->num; i++)
    {
    	if(resp->info[i].len == AID_LEN)
    	{
    		add_mac_by_id(resp->info[i].att, resp->info[i].mac, 0, 0);
    		request_authenticate_node_address(
        		resp->info[i].mac, resp->info[i].att, resp->info[i].len, resp->info[i].depth);
    	}
    }
}
//-------------------------------------------------------------------------------------------------
