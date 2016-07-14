#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "sdk.h"
#include "uart.h"
#include "sdk_svc.h"
#include "comfunc.h"
#include "adaptor.h"
#include "macmap.h"
#include "crypt.h"
#include "update.h"
#include "protocol.h"
#include "sec_printf.h"
#include "protocol_gateway.h"

//---------------------------------------------------------------------------------------
static int encode_9f_packet(uint8_t *buf, uint8_t *payload, uint16_t len, uint8_t ctrl)
{
    int dlen = 1 + 2 + len;
    int index = 0;
    buf[index++] = STC_R;
    // len
    buf[index++] = dlen & 0xFF;
    buf[index++] = (dlen >> 8) & 0xFF;
    //ctrl
    buf[index++] = ctrl;
    //info
    buf[index++] = 0x00;
    buf[index++] = 0x00;
    //payload
    memcpy(buf + index, payload, len);
    index += len;

    uint8_t cs = 0, cor = 0;
    int i;
    for (i = 3; i < index; i++)
    {
        cs += buf[i];
        cor ^= buf[i];
    }
    buf[index++] = cs;
    buf[index++] = cor;
    return encrypt(buf, index);
}
//---------------------------------------------------------------------------------------
static int creat_ask_packet(uint8_t *buf)
{
    uint8_t payload[] = { 0x00 };
    return encode_9f_packet(buf, payload, sizeof(payload), ACK_PACKET);
}
//---------------------------------------------------------------------------------------
static int read_operation_mode(uint8_t *buf)
{
    uint8_t payload[] = { READ_OPERATION_MODE, adaptor.type };
    return encode_9f_packet(buf, payload, sizeof(payload), LOCAL_OPERATION);
}
//---------------------------------------------------------------------------------------
static estimer_t tmr_reset;
static void on_tmr_reset(estimer_t tmr, void *arg)
{
    uint8_t tmp[0x40];
    uint8_t payload[] = { 0x90, 0x00 };
    int len =  encode_9f_packet(tmp, payload, sizeof(payload), LOCAL_OPERATION);
    esuart_send_data(UART_CHN_1, tmp, len);
    timer_delete(tmr_reset);
}
void notice_plc_chip_reset(void)
{
    tmr_reset = timer_create(200, 200, TMR_PERIODIC, on_tmr_reset, NULL, "tmr_reset");
    if (!tmr_reset)
    {
        pr_info("creat tmr_reset failed!\n");
        return;
    }

    timer_start(tmr_reset);
}
//---------------------------------------------------------------------------------------
static int read_all_info(uint8_t *buf)
{
    sdk_svc_mac_t mac;
    sdk_err_t err = sdk_svc_get_mac(&mac);
    if (err != 0)
    {
        pr_info("sdk svc get mac error\n");
        return -1;
    }
    //memcpy(adaptor.mac, mac.mac, MAC_LEN);
    uint8_t payload[] = { READ_ALL_INFO,                     //cmd
        0x45, 0x53,                                     //ES
        0x53, 0x53, 0x43, 0x16, 0x63,                   //SSC1663
        0x00, 0x00, 0x08, 0x00,                         //版本信息
        0x04,                                           //aid len
        mac.mac[0], mac.mac[1], mac.mac[2], mac.mac[3], //aid
        mac.mac[0], mac.mac[1], mac.mac[2], mac.mac[3], mac.mac[4], mac.mac[5], 0x00, 0x00, //eid
        adaptor.sid[0], adaptor.sid[1],                 //sid
        adaptor.panid[0], adaptor.panid[1],             //panid
        0x00,
        0x00
    };
    return encode_9f_packet(buf, payload, sizeof(payload), LOCAL_OPERATION);
}
//---------------------------------------------------------------------------------------
static void start_search(uint8_t *data)
{
    if (data[0] == 0x01)
    {
        sdk_svc_sch_start_t info;
        info.src_mod = SDK_MOD_6364;
        info.dst_mod = SDK_MOD_6364;
        info.radius = 15;
        info.rule = SDK_SCH_SAMENET;
        info.multi = SDK_MULTI_DIS;
        sdk_svc_sch_start(&info, adaptor.panid, PANID_LEN);
    }
}
//---------------------------------------------------------------------------------------
static int notice_plc_chip_panid(struct LbsFrame *frame)
{
	struct NoticePlcChipPanid * pPayload = (struct NoticePlcChipPanid *)frame->payload;
    if (!pPayload)
    {
        pr_info("notice plc chip panid payload is null\n");
        return -1;
    }

    memcpy(adaptor.aid, pPayload->aid, AID_LEN);
    memcpy(adaptor.panid, pPayload->panid, PANID_LEN);
    memcpy(adaptor.sid, pPayload->panid, PANID_LEN);
    
	clear_map();
    add_mac_by_id(adaptor.aid, adaptor.mac, adaptor.sid, 0);
    return do_set_psk();
}
//---------------------------------------------------------------------------------------
static int answer_authenticate_node_address(struct LbsFrame *frame)
{
	struct AnswerAuthenticate *pPayload = (struct AnswerAuthenticate *)frame->payload;
    if (!pPayload)
    {
        pr_info("answer authenticate node address payload is null\n");
        return -1;
    }
    if ((pPayload->aidLen & 0xE0) != 0)
    {
    	del_mac_by_id(pPayload->aid);
        pr_info("decline node\n");
        return -1;
    }
    uint8_t mac[MAC_LEN];
    int ret = get_mac_by_id(pPayload->aid, mac);
    if(ret >= 0)
    {
    	add_mac_by_id(pPayload->aid, mac, pPayload->sid, 0);
    	return 0;
    }
    
    struct RegData reg;
    memcpy(reg.aid, pPayload->aid, AID_LEN);
    memcpy(reg.panid, adaptor.panid, PANID_LEN);
    memcpy(reg.pw, pPayload->pwd, PW_LEN);
    memcpy(reg.gid, adaptor.aid, AID_LEN);
    memcpy(reg.sid, pPayload->sid, SID_LEN);

    uint8_t body[0x100];
    int len = code_body(0x0603, 0, &reg, sizeof(reg), body, sizeof(body));

    uint8_t tmp[0x100];
    len = code_frame(adaptor.aid, pPayload->aid, -1, CMD_SET, body, len, tmp, sizeof(tmp));

    if (do_plc_unicast(pPayload->eid, SDK_KEY_INIT, tmp, len) < 0)
        return -1;

    return 0;
}
//---------------------------------------------------------------------------------------
static void ask_node_quit_network(struct LbsFrame *frame)
{
	del_mac_by_id(&frame->payload[4]);			//模式1+数量1+节点类型1+aidLen1+aid
}
//---------------------------------------------------------------------------------------
void request_authenticate_node_address(const uint8_t *mac, uint8_t *att, uint8_t len, uint8_t hops)
{
    uint8_t buf[255];
    uint8_t payload[] = { hops+1, 0x00, 0x00, 0x00, 0x00,             //TNO
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],	0x00, 0x00, //EID
        0x00, 0x00,                                                 //注册SID
        0x00, 0x00,                                                 //中继SID
        0x00, 0x00, 0x10,                                           //注册节点版本号
        0x01,                                                       //aid count
        0x01,                                                       //aid type
        0x04,                                                       //aid len
        att[0], att[1], att[2], att[3],                             //aid
        0x01, 0x00
    };
    int ret = encode_9f_packet(buf, payload, sizeof(payload), REQUEST_AUTHENTICATE_NODE_ADDRESS);
    if (ret != -1)
        esuart_send_data(UART_CHN_1, buf, ret);
}
//---------------------------------------------------------------------------------------
static void set_operation_mode(uint8_t *data)
{
    adaptor.type = APP_TYPE_LBS;
}
//---------------------------------------------------------------------------------------
void node_ask_quit_network(uint8_t *aid)
{
    uint8_t buf[BUF_LEN];
    uint8_t payload[] = { 0x00, 0x01, 0x00, 0x04, aid[0], aid[1], aid[2], aid[3] };
    int ret = encode_9f_packet(buf, payload, sizeof(payload), NODE_ASK_QUIT_NETWORK);
    if (ret != -1)
        esuart_send_data(UART_CHN_1, buf, ret);
}
//---------------------------------------------------------------------------------------
int register_result(const uint8_t *src, struct Body *body)
{
	if (body->ctrl&0x80)
	{
		del_mac_by_id(body->data);
		node_ask_quit_network(body->data);
		return -NO_RETURN;
	}
    add_mac_by_id(body->data, src, &body->data[AID_LEN], 0);
    return -NO_RETURN;
}
//---------------------------------------------------------------------------------------
static void adaptor_upgrade(struct SmartFrame *frame)
{
    struct AppFrame *app = (struct AppFrame *)frame->data;
    int len = frame->len - sizeof(struct AppFrame);
    int ret = do_update(app->cmd, app->data, len);
    if (ret > 0) 
    {
        ret += sizeof(struct AppFrame);
        code_ret_frame(frame, ret);
        recv_node_info(adaptor.mac, frame);
    }
}
//---------------------------------------------------------------------------------------
static void notice_plc_reboot(struct SmartFrame *frame)
{
    if (is_all_xx(frame->taid, 0xFF, AID_LEN)) 
    {
        do_plc_broadcast(SDK_KEY_USER, frame, frame_len(frame));
        sdk_svc_reset();
    }
    else if (is_my_smart_frame(frame)) 
	{
		sdk_svc_reset();
        return;
	}

    uint8_t dst[6];
    if (get_mac_by_id(frame->taid, dst) >= 0)
    {
        do_plc_unicast(dst, SDK_KEY_USER, frame, frame_len(frame));
    }
}
//---------------------------------------------------------------------------------------
static int do_cmd(struct SmartFrame *pframe)
{
	struct AppFrame *app = (struct AppFrame *)pframe->data;
	struct Body *body;
	int count = 0, index = 0;
    uint8_t frame[BUF_LEN];
    uint8_t tmp[BUF_LEN];

    const char *soft_ver = get_dev_ver();
    const char *plc_ver = get_plc_ver();

	while(count < pframe->len - 1)
	{	
		body = (struct Body *)(app->data + count); 
		if(body->did[0] == 0x01 && body->did[1] == 0x06)
		{
			notice_plc_reboot(pframe);
			return -1;
		}
		if(body->did[0] == 0x02 && body->did[1] == 0x06)
		{
			int len = code_body(0x0602, 0, adaptor_soft_type, strlen(adaptor_soft_type), frame+index, sizeof(frame)-index);
			index += len;
		}
		if(body->did[0] == 0x06 && body->did[1] == 0x06)
		{
			int len = code_body(0x0606, 0, soft_ver, strlen(soft_ver), frame+index, sizeof(frame)-index);
			index += len;
		}
		if(body->did[0] == 0x09 && body->did[1] == 0x06)
		{
			int len = code_body(0x0609, 0, adaptor_plc_type, strlen(adaptor_plc_type), frame+index, sizeof(frame)-index);
			index += len;
		}
		if(body->did[0] == 0x0A && body->did[1] == 0x06)
		{
			int len = code_body(0x060A, 0, plc_ver, strlen(plc_ver), frame+index, sizeof(frame)-index);
			index += len;
		}
		count = count+2+1+(body->ctrl & 0x7F);
	}
	
	if(index != 0)
	{
		code_frame(adaptor.aid, adaptor.aid, pframe->seq|0x80, CMD_READ, frame, index, tmp, sizeof(tmp));
		recv_node_info(adaptor.mac, (struct SmartFrame *)tmp);
	}
	return 0;
}
//---------------------------------------------------------------------------------------
static int adaptor_operation(struct SmartFrame *pframe)
{
    struct AppFrame *app = (struct AppFrame *)pframe->data;
    if(app->cmd == 0x06)
    {
    	adaptor_upgrade(pframe);
    	return -1;
    }
    return do_cmd(pframe);
}
//---------------------------------------------------------------------------------------
static void send_node_info(struct SendNodeInfo *info)
{
    struct SmartFrame *pframe = (struct SmartFrame *)info->data;

    if (is_all_xx(info->tsid, 0xFF, sizeof(info->tsid))) 
    {
        do_plc_broadcast(SDK_KEY_USER, pframe, frame_len(pframe));
    }
    else if(is_my_smart_frame(pframe))
    {
    	adaptor_operation(pframe);
    }
    else
    {
        uint8_t dst[6];
        if (get_mac_by_id(pframe->taid, dst) < 0)
        {
            uint8_t body[0x100];
            int len = code_body(0x0605, 0, pframe->taid, sizeof(pframe->taid), body, sizeof(body));
            uint8_t tmp[0x100];
            len = code_frame(adaptor.aid, pframe->taid, -1, CMD_READ, body, len, tmp, sizeof(tmp));
            do_plc_broadcast(SDK_KEY_INIT, tmp, len);
        }
        else
        {
            do_plc_unicast(dst, SDK_KEY_USER, pframe, frame_len(pframe));
        }
    }
}
//---------------------------------------------------------------------------------------
int recv_node_info(const uint8_t *mac, struct SmartFrame *pFrame)
{
    uint8_t sid[SID_LEN];
    if (get_sid_by_mac(mac, sid) < 0)
    {
        request_authenticate_node_address(mac, pFrame->said, AID_LEN, 0);
        return 0;
    }

    uint8_t buf[255];
    uint8_t payload[255];
    int index = 0;
    payload[index++] = 0x91;
    payload[index++] = 0x02;
    payload[index++] = sid[0];
    payload[index++] = sid[1];
    payload[index++] = 0x00;
    int len = SMART_FRAME_HEAD+pFrame->len+1;
    memcpy(payload + index, pFrame, len);
    int ret = encode_9f_packet(buf, payload, len + 5, REMOTE_OPERATION);
    if (ret != -1)
        esuart_send_data(UART_CHN_1, buf, ret);
    return 0;
}
//---------------------------------------------------------------------------------------
static void local_operation(struct LbsFrame *frame)
{
	struct Operation *pPayload = (struct Operation *)frame->payload;
    uint8_t buf[BUF_LEN] = { 0 };
    uint8_t cmd = pPayload->cmd;
    int ret = -1;
    switch (cmd)
    {
    case READ_OPERATION_MODE:
        ret = read_operation_mode(buf);
        break;
    case READ_ALL_INFO:
        ret = read_all_info(buf);
        break;
    case SET_OPERATION_MODE:
        set_operation_mode(pPayload->data);
        break;
    default:
        break;
    }
    if (ret != -1)
        esuart_send_data(UART_CHN_1, buf, ret);
}
//---------------------------------------------------------------------------------------
static void remote_operation(struct LbsFrame *frame)
{
	struct Operation *pPayload = (struct Operation *)frame->payload;
    uint8_t cmd = pPayload->cmd;
    switch (cmd)
    {
    case NETWORK_CONTROL:
        start_search(pPayload->data);
        break;
    case SNED_NODE_INFO:
        send_node_info((struct SendNodeInfo *)pPayload->data);
        break;
    case RECV_NODE_INFO:
        break;
    default:
        break;
    }
}
//---------------------------------------------------------------------------------------
struct LbsFrame* get_lbs_frame(const uint8_t *in, int len)
{
    uint8_t *frm = (unsigned char *)memchr(in, STC_R, len);
    if (frm == NULL || frm[0] != STC_R || (len - (frm - in)) < sizeof(struct LbsFrame))
    {
        return NULL;
    }
    if (decrypt(frm, len - (frm - in)) == -1)
    {
        return get_lbs_frame(frm+1, len-(frm - in)-1);
    }
    return (struct LbsFrame *)frm;
}
//---------------------------------------------------------------------------------------
int lbs_frame_handle(struct LbsFrame *frame)
{
    if (!frame)
        return -1;
    uint8_t buf[BUF_LEN];
    int ret = creat_ask_packet(buf);
    if (ret != -1)
        esuart_send_data(UART_CHN_1, buf, ret);
    uint8_t ctrl = frame->ctrl;
    switch (ctrl)
    {
    case NOTICE_PLC_CHIP_PANID:
        notice_plc_chip_panid(frame);
        break;
    case ANSWER_AUTHENTICATE_NODE_ADDRESS:
        answer_authenticate_node_address(frame);
        break;
    case ASK_NODE_QUIT_NETWORK:
    	ask_node_quit_network(frame);
    	break;
    case LOCAL_OPERATION:
        local_operation(frame);
        break;
    case REMOTE_OPERATION:
        remote_operation(frame);
        break;
    default:
        break;
    }
    return 0;
}
//---------------------------------------------------------------------------------------
