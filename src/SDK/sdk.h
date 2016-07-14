/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		sdk.h
Description:	The general interface for high layer.
Author:			eastsoft
Version:		v1.0
Date:			2016/01/25
History:
*/
/************************************************************************************************/

#ifndef _SDK_H_
#define _SDK_H_

#define SDKaMACLen			6
#define SDKaSearMaxRptclt	5
#define SDKaMaxLinkNum		10
#define SDKaMaxGpidNum		3
#define SDKaMaxPskLen		64									/* ������������ */
#define SDKaMinPskLen		8									/* ��С��������� */

typedef void * func_t;
typedef void ** func_ptr_t;
typedef void ** func_tbl_t;
typedef void *** funcptr_tbl_t;

#define SDK_IMAGE_SIZE			(384 * 1024)				/* �����ļ��洢��384KB */
extern u8 *sdk_update_image;								/* �̼������ʼ��ַ, ��СΪSDK_IMAGE_SIZE. */

typedef enum _sdk_err_t										/* ִ��״̬��. */
{
	SDK_ERR_OK			= 0,								/* ��������״̬. */
	SDK_ERR_PARA		= -1,								/* ���ز�������. */
	SDK_ERR_BUF			= -2,								/* �����ڴ治��. */
	SDK_ERR_STATE		= -3,								/* ����״̬����. */
	SDK_ERR_LEN			= -4,								/* ���س��ȴ���. */
	SDK_ERR_FLASH		= -5,								/* ��дFlash����. */

	SDK_ERR_DEF,											/* ��Ч״̬. */
} sdk_err_t;

typedef enum _sdk_addr_mod_t								/* ��ַģʽ. */
{
	SDK_MOD_ANY			= 0x00,								/* �������豸 */
	SDK_MOD_GW			= 0x01,								/* �����豸 */
	SDK_MOD_CLOUD		= 0x02,								/* �ƶ��豸 */
	SDK_MOD_6364		= 0x03,								/* SSC1663/64�豸 */
	SDK_MOD_42			= 0x04,								/* SSC1642�豸 */
	SDK_MOD_BLE			= 0x05,								/* BLE�豸 */

	SDK_MOD_DEF,											/* ��Ч��ַģʽ. */
} sdk_addr_mod_t;

typedef enum _sdk_prio_t									/* ���ȼ�. */
{
	SDK_PRIO_LOW		= 0x00,								/* �����ȼ�. */
	SDK_PRIO_HIGH		= 0x01,								/* �����ȼ�. */

	SDK_PRIO_DEF,											/* ��Ч���ȼ�. */
} sdk_prio_t;

typedef enum _sdk_key_mod_t									/* ��Կģʽ. */
{
	SDK_KEY_INIT		= 0x00,								/* ��ʼ��Կ. */
	SDK_KEY_USER		= 0x01,								/* �û���Կ. */

	SDK_KEY_DEF,											/* ��Ч��Կ״̬. */
} sdk_key_mod_t;

typedef enum _sdk_multi_t									/* �鲥״̬. */
{
	SDK_MULTI_DIS		= 0x00,								/* ���鲥. */
	SDK_MULTI_EN		= 0x01,								/* �鲥. */

	SDK_MULTI_DEF,											/* ��Ч�鲥״̬. */
} sdk_multi_t;

typedef enum _sdk_sch_rule_t								/* �豸��������. */
{
	SDK_SCH_ANY			= 0x00,								/* ���������豸. */
	SDK_SCH_SAMENET		= 0x01,								/* ����ͬ�����豸. */
	SDK_SCH_NEW			= 0x02,								/* ���������豸. */

	SDK_SCH_DEF,											/* ��Ч��������. */
} sdk_sch_rule_t;

typedef enum _sdk_sch_state_t								/* �Ƿ�����豸����. */
{
	SDK_SCH_ABSENT		= 0x00,								/* �������豸����. */
	SDK_SCH_PARTICIPATE	= 0x01,								/* �����豸����. */

	SDK_SCH_STATE_DEF,										/* ��Ч״̬. */
} sdk_sch_state_t;

typedef enum _sdk_rmtpsk_state_t							/* Զ�����ÿ���ظ�״̬. */
{
	SDK_RMTPSK_REFUSE	= 0x00,								/* �ܾ��޸�. */
	SDK_RMTPSK_AGREE	= 0x01,								/* ͬ���޸�. */

	SDK_RMTPSK_DEF,											/* ��Ч״̬. */
} sdk_rmtpsk_state_t;

typedef enum _sdk_rmtsgid_state_t							/* Զ���޸��鲥��ַ�ظ�״̬. */
{
	SDK_RMTSGID_REFUSE	= 0x00,								/* �ܾ��޸�. */
	SDK_RMTSGID_AGREE	= 0x01,								/* ͬ���޸�. */

	SDK_RMTSGID_DEF,										/* ��Ч״̬. */
} sdk_rmtsgid_state_t;

typedef enum _sdk_tone_t									/* ���ز�����. */
{
	SDK_TONE_131		= 0x00,								/* 131���ز�. */
	SDK_TONE_411		= 0x01,								/* 411���ز�. */

	SDK_TONE_DEF,											/* ��Ч״̬. */
} sdk_tone_t;

typedef enum _sdk_ge_bridge_t								/* 64��ʼ��Ϊ����. */
{
	SDK_BRIDGE_OFF		= 0x00,								/* ��ʼ��Ϊ��ͨ�豸. */
	SDK_BRIDGE_ON		= 0x01,								/* ��ʼ��Ϊ����. */

	SDK_BRIDGE_DEF,											/* ��Ч״̬. */
} sdk_ge_bridge_t;

typedef enum _sdk_ge_dhcp_t									/* DHCPʹ�ܿ���״̬. */
{
	SDK_DHCP_INIT_OFF	= 0x00,								/* ʹ��TCP/IP�󲻿���DCHP. */
	SDK_DHCP_INIT_ON	= 0x01,								/* ʹ��TCP/IP����DCHP. */

	SDK_DHCP_INIT_DEF,										/* ��Ч״̬. */
} sdk_ge_dhcp_t;

typedef enum _sdk_rmtpsk_dec_t								/* Զ�����ÿ������״̬. */
{
	SDK_DEC_FAIL		= 0x00,								/* �޷�����. */
	SDK_DEC_NEW			= 0x01,								/* �����¿���, ���ڵ��ʱʹ�þɿ���. */
	SDK_DEC_OLD			= 0x02,								/* ���ܾɿ���, ���ڵ��ʱʹ���¿���. */

	SDK_DEC_DEF,											/* ��Ч״̬. */
} sdk_rmtpsk_dec_t;

typedef enum _sdk_link_sta_t								/* ����״̬. */
{
	SDK_LINK_NONE		= 0x00,								/* ������. */
	SDK_LINK_LINKING	= 0x01,								/* ������. */
	SDK_LINK_CANT		= 0x02,								/* �޷�����. */
	SDK_LINK_LOSE		= 0x03,								/* ���Ӷ�ʧ. */
	SDK_LINK_COMP		= 0x04,								/* ���ӽ���. */

	SDK_LINK_STA_DEF,										/* ��Ч״̬. */
} sdk_link_sta_t;

typedef enum _sdk_link_type_t								/* ��������. */
{
	SDK_LINK_UDP		= 0x00,								/* UDP����. */
	SDK_LINK_TCP		= 0x01,								/* TCP����. */

	SDK_LINK_TYPE_DEF,										/* ��Ч״̬. */
} sdk_link_type_t;

typedef enum _sdk_tx_state_t								/* �����߷���״̬. */
{
	SDK_TX_DATAF		= 0x00,								/* ����Ч·�� */
	SDK_TX_CSMA			= 0x01,								/* CSMAʧ�� */
	SDK_TX_DATERR		= 0x02,								/* ͨ��ʧ�ܻظ�����֡ */

	SDK_TX_DEF,												/* ��Ч״̬. */
} sdk_tx_state_t;

typedef enum _sdk_net_state_t								/* ����״̬. */
{
	SDK_NET_NONE		= 0x00,								/* ������. */
	SDK_NET_SAME		= 0x01,								/* ͬ����. */
	SDK_NET_OTHER		= 0x02,								/* ��������. */
	SDK_NET_UNKNOWN		= 0x03,								/* ״̬δ֪. */

	SDK_NET_DEF,											/* ��Ч״̬. */
} sdk_net_state_t;

typedef enum _sdk_tst_cmd_t									/* ����ָ��������. */
{
	SDK_TST_SFO			= 0x00,								/* Ƶƫͬ��. */
	SDK_TST_GET_SFO		= 0x01,								/* ��ȡƵƫ. */
	SDK_TST_REQ			= 0x02,								/* ���ܲ������ݷ�������. */
	SDK_TST_GAIN		= 0x03,								/* ��ȡRX GAIN/TX GAIN. */
	SDK_TST_GETNB		= 0x04,								/* ��ȡ����ģ���ַ */
	SDK_TST_CLKNB		= 0x05,								/* �������ģ���ַ */

	SDK_TST_DEF,											/* ��Ч����������. */
} sdk_tst_cmd_t;

typedef struct _sdk_skt_t									/* �׽���. */
{
	u32					ip;									/* IP. */
	u16					port;								/* �˿�. */
} sdk_skt_t;

typedef struct _sdk_skt_pair_t								/* �׽��ֶ�. */
{
	sdk_skt_t			src;								/* Դ�׽���. */
	sdk_skt_t			dst;								/* Ŀ���׽���. */
} sdk_skt_pair_t;

typedef struct _sdk_link_state_t							/* ����״̬. */
{
	sdk_skt_pair_t		skt;								/* �׽��ֶ�. */
	sdk_link_sta_t		sta;								/* ״̬. */
} sdk_link_state_t;

typedef struct _sdk_tmi_t									/* TMI����. */
{
	u8					mm;									/* ���Ʒ�ʽ. */
	u8					copy;								/* �ظ�����, ȡֵ1~12, Ŀǰֻ֧��1, 2, 4, 5, 7, 11. */
	u8					fec;								/* FEC������. */
} sdk_tmi_t;

typedef struct _sdk_frm_t									/* FRM֡����Ϣ. */
{
	u8					pb_amt;								/* PB����, ���15��, Ŀǰ����Ϊ4��. */
	u8					pb_type;							/* PB����. 0-136pb, 1-520pb*/
} sdk_frm_t;

#endif
