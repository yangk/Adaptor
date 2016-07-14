/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		sdk_evt.h
Description:	The event interface for high layer.
Author:			eastsoft
Version:		v1.0
Date:			2016/01/25
History:
*/
/************************************************************************************************/

#ifndef _SDK_EVT_H_
#define _SDK_EVT_H_

#include "sdk.h"

typedef struct _sdk_evt_tx_state_t							/* �����߷����쳣״̬�ϱ�����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	u8					dst[SDKaMACLen];					/* Ŀ��MAC��ַ. */
	sdk_tx_state_t		state;								/* ����״̬. */
} sdk_evt_tx_state_t;

typedef struct _sdk_evt_rx_plc_t							/* �����߽������ݲ���. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	u8					depth;								/* ���. */
	sdk_key_mod_t		key_mod;							/* ��Կģʽ. */
	sdk_multi_t			multi;								/* �鲥��־. */
	u16					src_gpid;							/* Դ�ڵ����ַ. */
	u8					src[SDKaMACLen];					/* Դ�ڵ�MAC��ַ. */
} sdk_evt_rx_plc_t;

typedef struct _sdk_evt_devprop_req_t						/* �����豸�������Բ���. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_multi_t			multi;								/* �鲥��־. */
	u16					dst_gpid;							/* Ŀ�Ľڵ����ַ. */
	u8					src[SDKaMACLen];					/* Դ�ڵ�MAC��ַ. */
	u8					taskid;								/* �������������. */
} sdk_evt_devprop_req_t;

typedef struct _sdk_evt_node_info_t							/* �ڵ���Ϣ����. */
{
	u8					*mac;								/* ���ѽڵ�MAC��ַ. */
	u8					depth;								/* ���ѽڵ����. */
	sdk_net_state_t		net;								/* ���ѽڵ�����״̬. */
	sdk_addr_mod_t		mod;								/* ���ѽڵ��ַģʽ. */
	sdk_multi_t			multi;								/* �鲥��־. */
	u16					gpid;								/* Դ�ڵ����ַ. */
	u8					*att;								/* ���ѽڵ��ϱ�����������. */
	u8					len;								/* �������ݳ���. */
} sdk_evt_node_info_t;

typedef struct _sdk_evt_sch_rpt_t							/* �ϱ������豸����. */
{
	u8					num;								/* �������Ľڵ�����. */
	sdk_evt_node_info_t	info[SDKaSearMaxRptclt];			/* ���ѽڵ���Ϣ. */
} sdk_evt_sch_rpt_t;

typedef struct _sdk_evt_rmtpsk_req_t						/* �ϱ������豸����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	u8					depth;								/* ���. */
	sdk_multi_t			multi;								/* �鲥��ַ��־ */
	u16					gid;								/* �鲥��ַ */
	sdk_rmtpsk_dec_t	state;								/* ����״̬. */
	u8					src[SDKaMACLen];					/* Դ�ڵ�MAC��ַ. */
} sdk_evt_rmtpsk_req_t;

typedef struct _sdk_evt_rmtpsk_resp_t						/* Զ�����ÿ���ظ��ϱ�����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	u8					depth;								/* ���. */
	sdk_multi_t			multi;								/* �鲥��ַ��־ */
	u16					gid;								/* �鲥��ַ */
	u8					src[SDKaMACLen];					/* Դ�ڵ�MAC��ַ. */
	sdk_rmtpsk_state_t	state;								/* �Ƿ�ͬ���޸Ŀ���. */
} sdk_evt_rmtpsk_resp_t;

typedef struct _sdk_evt_rmtsgid_req_t 						/* Զ���޸��鲥��ַ�����ϱ�����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	u8					depth;								/* ��� */
	sdk_multi_t			multi;								/* �鲥��ַ��־ */
	u16					gid;								/* �鲥��ַ */
	u8					src[SDKaMACLen];					/* Դ�ڵ�MAC��ַ. */
	sdk_rmtsgid_state_t	state;								/* �鲥��ַ�Ƿ���Ч */
} sdk_evt_rmtsgid_req_t;

typedef struct _sdk_evt_rmtsgid_resp_t						/* Զ���޸��鲥��ַ�ظ��ϱ�����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	u8					depth;								/* ���. */
	sdk_multi_t			multi;								/* �鲥��ַ��־ */
	u16					gid;								/* �鲥��ַ */
	u8					src[SDKaMACLen];					/* Դ�ڵ�MAC��ַ. */
	sdk_rmtsgid_state_t	state;								/* �Ƿ�ͬ���޸Ŀ���. */
} sdk_evt_rmtsgid_resp_t;

typedef struct _sdk_evt_tst_resp_t							/* ���Ͳ�������Ӧ��. */
{
	u8					src[SDKaMACLen];					/* Դ��ַ. */
	sdk_tmi_t			tmi;								/* TMI. */
	sdk_frm_t			frm;								/* FRM ��Ϣ. */
	s8					rgain;								/* ���չ���. */
} sdk_evt_tst_resp_t;

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_tx_state
	Description	:	�����߷����쳣״̬�ϱ�
	Input		:	state - �ϱ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_tx_state(const sdk_evt_tx_state_t *state);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rx_plc
	Description	:	�����߽��������ϱ�
	Input		:	info - ���ղ���
					data - ��������
					len  - ���ݳ���
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rx_plc(const sdk_evt_rx_plc_t *info, const u8 *data, const u32 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_devprop_req
	Description	:	�����豸��������
	Input		:	req - �����豸�������Բ���
					att - �����豸������������
					len - �����豸�����������ݳ���
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_devprop_req(const sdk_evt_devprop_req_t *req, const u8 *att, const u8 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_sch_rpt
	Description	:	�ϱ������豸
	Input		:	resp - �ϱ������豸����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_sch_rpt(const sdk_evt_sch_rpt_t *resp);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmtpsk_req
	Description	:	Զ�����ÿ��������ϱ�
	Input		:	req - �������
					key - �¿���
					len - �¿����
	Output		:	none
	Return		:	none
	Notes		:	ֻ�н���״̬ΪSDK_DEC_NEWʱ, key��len��Ч
*/
/*****************************************************************************************************/
extern void sdk_evt_rmtpsk_req(const sdk_evt_rmtpsk_req_t *req, const u8 *key, const u8 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmtpsk_resp
	Description	:	Զ�����ÿ���ظ��ϱ�
	Input		:	resp - �ظ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rmtpsk_resp(const sdk_evt_rmtpsk_resp_t *resp);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmthiepsk_req
	Description	:	Զ���������ɿ��������ϱ�
	Input		:	req - �������
					key - �¿���
					len - �¿����
	Output		:	none
	Return		:	none
	Notes		:	ֻ�н���״̬ΪSDK_DEC_NEWʱ, key��len��Ч
*/
/*****************************************************************************************************/
extern void sdk_evt_rmthiepsk_req(const sdk_evt_rmtpsk_req_t *req, const u8 *key, const u8 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmthiepsk_resp
	Description	:	Զ�����ÿ���ظ��ϱ�
	Input		:	resp - �ظ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rmthiepsk_resp(const sdk_evt_rmtpsk_resp_t *resp);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmtpsk_resp
	Description	:	Զ���޸��鲥��ַ�����ϱ�
	Input		:	resp - �ظ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rmtsgid_req (const sdk_evt_rmtsgid_req_t *req, const u8 cnt, const u8 *gid);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmtpsk_resp
	Description	:	Զ���޸��鲥��ַ�ظ��ϱ�
	Input		:	resp - �ظ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rmtsgid_resp(const sdk_evt_rmtsgid_resp_t *resp);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_dns
	Description	:	DNS��������ϱ�
	Input		:	name - ������
					len  - ����������
					ip   - ����IP
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_dns(const u8 *name, const u8 len, const u32 ip);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_skt_state
	Description	:	����״̬�ϱ�
	Input		:	sta - ����״̬
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_skt_state(const sdk_link_state_t *sta);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rx_ge
	Description	:	���ڽ��������ϱ�
	Input		:	pair - �׽��ֶ�
					data - ��������
					len  - �������ݳ���
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rx_ge(const sdk_skt_pair_t *pair, const u8 *data, const u32 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_tx_ledon
	Description	:	PLC TX����
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_tx_ledon(void);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_tx_ledoff
	Description	:	PLC TX�ص�
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_tx_ledoff(void);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rx_ledon
	Description	:	PLC RX����
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rx_ledon(void);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rx_ledon
	Description	:	PLC RX�ص�
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_rx_ledoff(void);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_tst_resp
	Description	:	���Ͳ������ݽ���
	Input		:	resp - ���ղ���
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_tst_resp(const sdk_evt_tst_resp_t *resp);

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_getsec_verdate
	Description	:	��ȡ���ο����ϲ�İ汾�źͰ汾����
	Input		:	version:�汾�ţ�2�ֽ�С��
					verdate:�汾���ڣ���������ɵ�3�ֽڰ汾���ڣ�С��
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_evt_getsec_ver(u8 *version, u8 *verdate);

#endif
