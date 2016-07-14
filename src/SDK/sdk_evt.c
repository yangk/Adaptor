/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		sdk_evt.c
Description:	The event interface for high layer.
Author:			eastsoft
Version:		v1.0
Date:			2016/01/25
History:
*/
/************************************************************************************************/

#include "include.h"
#include "sdk_evt.h"
#include "comfunc.h"
#include "adaptor.h"
#include "gpio.h"

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
void sdk_evt_tx_state(const sdk_evt_tx_state_t *state)
{

}

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
void sdk_evt_rx_plc(const sdk_evt_rx_plc_t *info, const u8 *data, const u32 len)
{
    adaptor_on_evt_rx_plc(info, data, len);
}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_devprop_req
	Description	:	�����豸��������
	Input		:	req - �������
					att - �����е���������
					len - �������ݳ���
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
void sdk_evt_devprop_req(const sdk_evt_devprop_req_t *req, const u8 *att, const u8 len)
{
    adaptor_on_evt_devprop_req(req, att, len);
}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_sch_rpt
	Description	:	�ϱ������豸
	Input		:	resp - �ظ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
void sdk_evt_sch_rpt(const sdk_evt_sch_rpt_t *resp)
{
	adaptor_evt_sch_rpt(resp);
}

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
void sdk_evt_rmtpsk_req(const sdk_evt_rmtpsk_req_t *req, const u8 *key, const u8 len)
{

}

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
void sdk_evt_rmtpsk_resp(const sdk_evt_rmtpsk_resp_t *resp)
{

}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmtsgid_req
	Description	:	Զ���޸��鲥��ַ�����ϱ�
	Input		:	req - �������
					cnt - �鲥��ַ����
					gid - �鲥��ַ�б�
	Output		:	none
	Return		:	none
	Notes		:	
*/
/*****************************************************************************************************/
void sdk_evt_rmtsgid_req(const sdk_evt_rmtsgid_req_t *req, const u8 cnt, const u8 *gid)
{

}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rmtsgid_resp
	Description	:	Զ���޸��鲥��ַ�ظ��ϱ�
	Input		:	resp - �ظ�����
	Output		:	none
	Return		:	none
	Notes		:	none
*/
/*****************************************************************************************************/
void sdk_evt_rmtsgid_resp(const sdk_evt_rmtsgid_resp_t *resp)
{

}

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
void sdk_evt_rmthiepsk_req(const sdk_evt_rmtpsk_req_t *req, const u8 *key, const u8 len)
{

}

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
void sdk_evt_rmthiepsk_resp(const sdk_evt_rmtpsk_resp_t *resp)
{

}

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
void sdk_evt_dns(const u8 *name, const u8 len, const u32 ip)
{

}

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
void sdk_evt_skt_state(const sdk_link_state_t *sta)
{

}

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
void sdk_evt_rx_ge(const sdk_skt_pair_t *pair, const u8 *data, const u32 len)
{

}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_tx_ledon
	Description	:	PLC TX����
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	�����ڲ�ֻ��GPIO��ת������λ,������ʱ�ȴ���,�����Ӱ������
*/
/*****************************************************************************************************/
void sdk_evt_tx_ledon(void)
{
#if 0
	gpio_out_high(TX_A_SEL);
	gpio_out_high(TX_B_SEL);
#endif
	gpio_out_high(TX_C_SEL);
}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_tx_ledoff
	Description	:	PLC TX�ص�
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	�����ڲ�ֻ��GPIO��ת������λ,������ʱ�ȴ���,�����Ӱ������
*/
/*****************************************************************************************************/
void sdk_evt_tx_ledoff(void)
{
#if 0
	gpio_out_low(TX_A_SEL);
	gpio_out_low(TX_B_SEL);
#endif
	gpio_out_low(TX_C_SEL);

}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rx_ledon
	Description	:	PLC RX����
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	�����ڲ�ֻ��GPIO��ת������λ,������ʱ�ȴ���,�����Ӱ������
*/
/*****************************************************************************************************/
void sdk_evt_rx_ledon(void)
{
	gpio_out_low(RX_PHS_LED);
}

/*****************************************************************************************************/
/*
	Function	:	sdk_evt_rx_ledoff
	Description	:	PLC RX�ص�
	Input		:	none
	Output		:	none
	Return		:	none
	Notes		:	�����ڲ�ֻ��GPIO��ת������λ,������ʱ�ȴ���,�����Ӱ������
*/
/*****************************************************************************************************/
void sdk_evt_rx_ledoff(void)
{
	gpio_out_high(RX_PHS_LED);
#if 0	
	gpio_out_high(RX_A_SEL);
	gpio_out_high(RX_B_SEL);
#endif
	gpio_out_high(RX_C_SEL);
}

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
void sdk_evt_tst_resp(const sdk_evt_tst_resp_t *resp)
{
    do_evt_tst_resp(resp);
}

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
void sdk_evt_getsec_ver(u8 *version, u8 *verdate)
{
    put_le_val(ADAPTOR_VERSION, version, 2);
    put_le_val(ADAPTOR_DATE, verdate, 3); 
}
