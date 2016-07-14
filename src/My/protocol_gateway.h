#ifndef _PROTOCOL_GATEWAY_H_
#define _PROTOCOL_GATEWAY_H_

#include <stdint.h>
#include "protocol.h"

//---------------------------------------------------------------------------------------
#define STC_R           0x9F
#define	ERROR_PACKET	0x01						// 错误报文
#define ACK_PACKET 		0x02 						// 确认报文
#define NOTICE_PLC_CHIP_PANID		0x11		 	// 通知载波芯片网络PANID
#define REQUEST_AUTHENTICATE_NODE_ADDRESS 	0x12	// 请求认证节点应用地址的合法性
#define ANSWER_AUTHENTICATE_NODE_ADDRESS 	0x13 	// 应答节点应用地址认证结果
#define ASK_NODE_QUIT_NETWORK 		0x15			// 要求节点退出网络
#define NODE_ASK_QUIT_NETWORK 		0x16			// 节点主动退出网络通知
#define LOCAL_OPERATION 		0x20				// 本地操作指令
#define REMOTE_OPERATION 		0x21				// 远程操作指令

#define	READ_OPERATION_MODE		0x10				// 读取操作模式
#define	READ_ALL_INFO			0x11				// 读取综合信息
#define	SET_OPERATION_MODE		0x50				// 设置操作模式

#define NETWORK_CONTROL			0X01				// 网络控制
#define SNED_NODE_INFO			0X10				// 向某节点发送信息
#define RECV_NODE_INFO			0X11				// 收到某节点信息

//---------------------------------------------------------------------------------------

#pragma pack(1)


struct LbsFrame
{
	uint8_t head;
	uint16_t len;
	uint8_t ctrl;
	uint16_t info;
	uint8_t payload[0];
};

struct NoticePlcChipPanid
{
	uint8_t sno;
	uint8_t aidLen;
	uint8_t aid[AID_LEN];
	uint8_t panid[PANID_LEN];
	uint8_t bsn;
	uint8_t gmk[4];
};

struct AnswerAuthenticate
{
	uint8_t tno[5];
	uint8_t eid[EID_LEN];
	uint8_t sid[SID_LEN];
	uint8_t gid[GID_LEN];
	uint8_t aidCount;
	uint8_t aidType;
	uint8_t aidLen;
	uint8_t aid[AID_LEN];
	uint8_t pwdLen;
	uint8_t pwd[2];
};

struct AnswerSidPanid
{
	uint8_t panid[PANID_LEN];
	uint8_t pwd[PW_LEN];
	uint8_t gatewayAid[AID_LEN];
	uint8_t sid[SID_LEN];
};

struct Operation
{
	uint8_t cmd;
	uint8_t data[0];
};

//struct ReadAllInfo
//{
//	uint8_t cmd;
//	uint8_t firm[2];
//	uint8_t chip[5];
//	uint8_t version[4];	
//	uint8_t aidLen;	
//	uint8_t aid[AID_LEN];
//	uint8_t eid[EID_LEN];
//	uint8_t sid[SID_LEN];
//	uint8_t panid[PANID_LEN];
//	uint8_t mode;
//};

struct SendNodeInfo
{
	uint8_t mode;
	uint8_t tsid[2];
	uint8_t relaySid[2];
	uint8_t data[0];
};

struct RecvNodeInfo
{
	uint8_t cmd;
	uint8_t mode;
	uint8_t sid[2];
	uint8_t hops;
	uint8_t data[0];
};

#pragma pack()

struct LbsFrame * get_lbs_frame(const uint8_t *in, int len);
int lbs_frame_handle(struct LbsFrame *frame);

void request_authenticate_node_address(const uint8_t *mac, uint8_t *att, uint8_t len, uint8_t hops);
int recv_node_info(const uint8_t *mac, struct SmartFrame *pFrame);
void node_ask_quit_network(uint8_t *aid);
void notice_plc_chip_reset();
int register_result(const uint8_t *src, struct Body *body);
#endif
