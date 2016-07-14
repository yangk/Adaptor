/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		sdk_svc.h
Description:	The service interface for high layer.
Author:			eastsoft
Version:		v1.0
Date:			2016/01/25
History:
*/
/************************************************************************************************/

#ifndef _SDK_SVC_H_
#define _SDK_SVC_H_

#include "sdk.h"

typedef struct _sdk_svc_ver_t								/* �汾��Ϣ. */
{
	u16					manu;								/* �����̱�־,����:"ES"(0x4553). */
	u8					chl;								/* �ŵ�����. */
	u8					mod;								/* ģ������. */
	u16					config;								/* ������Ϣ. */
	u16					ver;								/* �汾��. */
	u32					date;								/* �汾ʱ��. */
} sdk_svc_ver_t;

typedef struct _sdk_svc_mac_t								/* MAC��ַ. */
{
	u8					mac[SDKaMACLen];					/* MAC��ַ. */
} sdk_svc_mac_t;

typedef struct _sdk_svc_gpid_t								/* ���ַ. */
{
	u8					cnt;								/* ���ַ����. */
	u16					gpid[SDKaMaxGpidNum];				/* �鲥��ַ����. */
} sdk_svc_gpid_t;

typedef struct _sdk_svc_psk_t								/* ��Կ. */
{
	u8					psk[64];							/* ��Կ. */
	u8					len;								/* ��Կ����. */
} sdk_svc_psk_t;

typedef struct _sdk_svc_uni_t								/* ��������. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_prio_t			prio;								/* ���ȼ�. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	sdk_key_mod_t		key_mod;							/* ��Կģʽ. */
	u16					dst_gpid;							/* �鲥��ַ. */
	u8					dst[SDKaMACLen];					/* Ŀ��MAC. */
} sdk_svc_uni_t;

typedef struct _sdk_svc_brd_t								/* �㲥����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_prio_t			prio;								/* ���ȼ�. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	sdk_key_mod_t		key_mod;							/* ��Կģʽ. */
	u16					dst_gpid;							/* Ŀ���鲥��ַ */
	u8					radius;								/* �㲥�뾶. */
} sdk_svc_brd_t;

typedef struct _sdk_svc_sch_start_t							/* ��ʼ�豸��������. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_sch_rule_t		rule;								/* �豸��������. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	u16					dst_gpid;							/* �鲥��ַ. */
	u8					radius;								/* �豸�����뾶. */
} sdk_svc_sch_start_t;

typedef struct _sdk_svc_devprop_resp_t						/* �ظ��豸�������Բ���. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_sch_state_t		state;								/* �Ƿ�����豸����. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	u16					src_gpid;							/* �鲥��ַ. */
	u8					src[SDKaMACLen];					/* �����豸�����ڵ��ַ. */
	u8					taskid;								/* �����豸���������. */
} sdk_svc_devprop_resp_t;

typedef struct _sdk_svc_rmtpsk_req_t						/* Զ�����ÿ����������. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_prio_t			prio;								/* ��������ȼ�. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	u16					dst_gpid;							/* �鲥��ַ. */
	u8					dst[SDKaMACLen];					/* Ŀ��MAC��ַ. */
	u8					*old_key;							/* Ŀ�Ľڵ�ľɿ���. */
	u8					old_len;							/* �ɿ����. */
	u8					*new_key;							/* �¿���. */
	u8					new_len;							/* �¿����. */
} sdk_svc_rmtpsk_req_t;

typedef struct _sdk_svc_rmtpsk_resp_t						/* Զ�����ÿ���ظ�����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_prio_t			prio;								/* �ظ������ȼ�. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	u16					dst_gpid;							/* �鲥��ַ. */
	u8					dst[SDKaMACLen];					/* ��������Ľڵ��MAC��ַ. */
	sdk_rmtpsk_state_t	state;								/* �Ƿ�ͬ���޸Ŀ���. */
} sdk_svc_rmtpsk_resp_t;

typedef struct _sdk_svc_rmtsgid_req_t						/* Զ���޸��鲥��ַ�������. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	u16					dst_gpid;							/* �鲥��ַ. */
	u8					dst[SDKaMACLen];					/* Ŀ��MAC��ַ. */
	u8					gidcnt;								/* �鲥��ַ */
	u16					gid[SDKaMaxGpidNum];				/* �鲥��ַ�б� */
} sdk_svc_rmtsgid_req_t;

typedef struct _sdk_svc_rmtsgid_resp_t						/* Զ���޸��鲥��ַ�ظ�����. */
{
	sdk_addr_mod_t		src_mod;							/* Դ��ַģʽ. */
	sdk_addr_mod_t		dst_mod;							/* Ŀ�ĵ�ַģʽ. */
	sdk_multi_t			multi;								/* �鲥��ַ��־. */
	u16					dst_gpid;							/* �鲥��ַ. */
	u8					dst[SDKaMACLen];					/* ��������Ľڵ��MAC��ַ. */
	sdk_rmtsgid_state_t	state;								/* �Ƿ�ͬ���޸��鲥��ַ. */
} sdk_svc_rmtsgid_resp_t;

typedef struct _sdk_svc_ge_init_t							/* GE��ʼ������. */
{
	sdk_ge_bridge_t		bridge;								/* �Ƿ��ʼ��Ϊ����, ֻ��64��Ч. */
	sdk_ge_dhcp_t		dhcp;								/* ��ʼ�����Ƿ�ֱ�ӿ���DHCP. 64,63����. */
} sdk_svc_ge_init_t;

typedef struct _sdk_svc_ge_info_t							/* ������Ϣ����. */
{
	u32					ip;									/* IP��ַ. */
	u32					mask;								/* ��������. */
	u32					gw;									/* ����. */
	u32					fri_dns;							/* ��ѡDNS������. */
	u32					sec_dns;							/* ����DNS������. */
} sdk_svc_ge_info_t;

typedef struct _sdk_svc_que_ge_t							/* ��ȡ���Ӳ���. */
{
	u8					num;								/* ��ȡ������������. */
	sdk_link_state_t	link[SDKaMaxLinkNum];				/* ������Ϣ. */
} sdk_svc_que_ge_t;

typedef struct _sdk_svc_tst_sfo_t							/* Ƶƫ֪ͨ���Բ���. */
{
	u8					dst[SDKaMACLen];					/* �������, Ŀ��MAC��ַ. */
} sdk_svc_tst_sfo_t;

typedef struct _sdk_svc_tst_get_sfo_t						/* ��ȡƵƫ֪ͨ���Բ���. */
{
	u8					dst[SDKaMACLen];					/* �������, Ŀ��MAC��ַ. */
	u16					sfo;								/* ��������, Ƶƫֵ. */
} sdk_svc_tst_get_sfo_t;

typedef struct _sdk_svc_tst_req_t							/* ���Ͳ�����������. */
{
	u8					dst[SDKaMACLen];					/* ����, Ŀ�ĵ�ַ. */
	sdk_tmi_t			tmi;								/* ����, TMI. */
	sdk_frm_t			frm;								/* ����, FRM ��Ϣ. */
} sdk_svc_tst_req_t;

typedef struct _sdk_svc_tst_gain_t							/* ��ȡgain����. */
{
	s8					rgain;								/* ����, ���չ���. */
	s8					tgain;								/* ����, ���͹���. */
} sdk_svc_tst_gain_t;

typedef struct _sdk_svc_tst_getnb_t
{
	u32					amt;								/* ����, ��Ҫ��ȡ��MAC��ַ�������, ���ֵΪ500. */
															/* ����, ʵ�ʻ�õ�MAC��ַ����, С�ڵ��ڴ���ֵ. */
	u8					*mac;								/* ����, ����ڴ�, size >= amt * 6. */
															/* ����, mac��ַ����. */
} sdk_svc_tst_getnb_t;

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_reset
	Description	:	����
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
extern void sdk_svc_reset(void);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_get_ver
	Description	:	��ȡ�汾��Ϣ
	Input		:	info - �洢�汾��Ϣ���ڴ�
	Output		:	info - ��ȡ���İ汾��Ϣ
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_get_ver(sdk_svc_ver_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_get_mac
	Description	:	��ȡMAC��ַ
	Input		:	info - �洢MAC��ַ���ڴ�
	Output		:	info - ��ȡ����MAC��ַ
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_get_mac(sdk_svc_mac_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_get_gpid
	Description	:	��ȡ���ַ
	Input		:	info - �洢���ַ���ڴ�
	Output		:	info - ��ȡ�������ַ
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_get_gpid(sdk_svc_gpid_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_set_gpid
	Description	:	�������ַ
	Input		:	info - ��Ҫ���õ����ַ
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_set_gpid(sdk_svc_gpid_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_get_psk
	Description	:	��ȡ���ؿ���
	Input		:	info - �洢���ؿ�����ڴ�
	Output		:	info - ��ȡ���ı��ؿ���
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_get_psk(sdk_svc_psk_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_set_psk
	Description	:	���ñ��ؿ���
	Input		:	info - ��Ҫ���õı��ؿ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_set_psk(sdk_svc_psk_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_get_hiepsk
	Description	:	��ȡ�������ɿ���
	Input		:	info - �洢�������ɿ�����ڴ�
	Output		:	info - ��ȡ���ı������ɿ���
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_get_hiepsk(sdk_svc_psk_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_set_hiepsk
	Description	:	���ñ������ɿ���
	Input		:	info - ��Ҫ���õı������ɿ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_set_hiepsk(sdk_svc_psk_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_set_mac
	Description	:	����MAC��ַ
	Input		:	info - ��Ҫ���õ�MAC��ַ
	Output		:	none
	Return		:	״̬��
	Notes		:	MAC��ַд��FLASH, �й�װ���ָ��ɾ��
					64����MAC��ַ��������
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_set_mac(sdk_svc_mac_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_unicast
	Description	:	�����ߵ���
	Input		:	info - ��������
					data - ��������
					len  - ���ݳ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_unicast(sdk_svc_uni_t *info, u8 *data, u32 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_brodcast
	Description	:	�����߹㲥
	Input		:	info - �㲥����
					data - �㲥����
					len  - ���ݳ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_brodcast(sdk_svc_brd_t *info, u8 *data, u32 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_sch_start
	Description	:	��ʼ�豸����
	Input		:	info - �豸��������
					att  - �����豸�����е���������
					len  - ���Գ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_sch_start(sdk_svc_sch_start_t *info, u8 *att, u8 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_sch_stop
	Description	:	ֹͣ�豸����
	Input		:	none
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_sch_stop(void);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_devprop_resp
	Description	:	�ظ��豸��������
	Input		:	info - �ظ��豸�������Բ���
					att  - �ظ��豸������������
					len  - ���Գ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_devprop_resp(sdk_svc_devprop_resp_t *info, u8 *att, u8 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_rmtpsk_req
	Description	:	Զ�����ÿ�������
	Input		:	info - �������
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_rmtpsk_req(sdk_svc_rmtpsk_req_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_rmtpsk_resp
	Description	:	Զ�����ÿ���ظ�
	Input		:	info - �ظ�����
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_rmtpsk_resp(sdk_svc_rmtpsk_resp_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_rmthiepsk_req
	Description	:	Զ���������ɿ�������
	Input		:	info - �������
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_rmthiepsk_req(sdk_svc_rmtpsk_req_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_rmthiepsk_resp
	Description	:	Զ���������ɿ���ظ�
	Input		:	info - �ظ�����
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_rmthiepsk_resp(sdk_svc_rmtpsk_resp_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_rmtsgid_req
	Description	:	Զ���޸��鲥��ַ����
	Input		:	info - �������
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_rmtsgid_req(sdk_svc_rmtsgid_req_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_rmtsgid_resp
	Description	:	Զ���޸��鲥��ַ�ظ�
	Input		:	info - �ظ�����
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_rmtsgid_resp(sdk_svc_rmtsgid_resp_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_set_tone
	Description	:	�����ز�����
	Input		:	tone  - ���ز�
					tgain - ���͹���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_set_tone(sdk_tone_t tone, int tgain);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_en_ge
	Description	:	ʹ��TCP/IPģʽ
	Input		:	info - ��ʼ������
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_en_ge(sdk_svc_ge_init_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_get_ge
	Description	:	��ȡ������Ϣ
	Input		:	info - �洢������Ϣ���ڴ�
	Output		:	info - ��ȥ����������Ϣ
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_get_ge(sdk_svc_ge_info_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_set_ge
	Description	:	����������Ϣ
	Input		:	info - ��Ҫ���õ�������Ϣ
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_set_ge(sdk_svc_ge_info_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_start_dns
	Description	:	��ʼDNS
	Input		:	host - ������
					len  - ����������
	Output		:	none
	Return		:	״̬��
	Notes		:	ͬһʱ��ֻ����һ��DNS����
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_start_dns(u8 *host, u8 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_que_ge
	Description	:	��ȡ����״̬
	Input		:	pair - �׽��ֶ�
					info - �洢����״̬���ڴ�
	Output		:	info - ��ȡ��������״̬
	Return		:	״̬��
	Notes		:	�׽��ֶ�ȫ0, ��ѯ��������; Ŀ��Ϊ0,
					ƥ��Դ�׽���; ԴΪ0, ƥ��Ŀ���׽���
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_que_ge(sdk_skt_pair_t *pair, sdk_svc_que_ge_t *info);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_link_ge
	Description	:	����TCP/UDP����
	Input		:	type - ��������
					pair - �׽��ֶ�
	Output		:	none
	Return		:	״̬��
	Notes		:	�����ض˿�Ϊ0, ���49153���Ϸ��䱾�ض˿ں�
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_link_ge(sdk_link_type_t type, sdk_skt_pair_t *pair);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_del_ge
	Description	:	ɾ������
	Input		:	pair - �׽��ֶ�
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_del_ge(sdk_skt_pair_t *pair);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_tx_ge
	Description	:	����TCP/UDP����
	Input		:	pair - �׽��ֶ�
					data - ���͵�����
					len  - ���ݳ���
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_tx_ge(sdk_skt_pair_t *pair, u8 *data, u32 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_update
	Description	:	�����̼�
	Input		:	len - ����������
	Output		:	none
	Return		:	״̬��
	Notes		:	�������ݱ������sdk_update_image[]��
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_update(u32 len);

/*****************************************************************************************************/
/*
	Function	:	sdk_svc_tst
	Description	:	����ָ��
	Input		:	cmd  - ����������
					para - �������	/sdk_svc_tst_sfo_t/sdk_svc_tst_get_sfo_t/sdk_svc_tst_req_t
									/sdk_svc_tst_gain_t
	Output		:	none
	Return		:	״̬��
	Notes		:	none
*/
/*****************************************************************************************************/
extern sdk_err_t sdk_svc_tst(sdk_tst_cmd_t cmd, void *para);

#endif
