/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		uart.h
Description:	uart control
Author:			ch
Version:		v1.0
Date:			2015/06/24
History:
*/
/************************************************************************************************/

#ifndef _UART_H
#define _UART_H

#include "timer.h"

/*
********************************************************************************
*							uart register definition
********************************************************************************
*/
typedef volatile struct UART_DEV_S {
	union {								/* 0xF2_*00000 */
		unsigned int rxb;				/* DLAB=0: receive buffer, read-only */
		unsigned int txb;				/* DLAB=0: transmit buffer, write-only */
		unsigned int dll;				/* DLAB=1: divisor, least-significant byte latch (was write-only?) */
	} w0;
	union								/* 0xF2_*00004 */
	{
		unsigned int ier;				/* DLAB=0: interrupt-enable register (was write-only?) */
		unsigned int dlm;				/* DLAB=1: divisor, most-significant byte latch (was write-only?) */
	} w1;

	union								/* 0xF2_*00008 */
	{
		unsigned int iir;				/* DLAB=0: interrupt information register, read-only */
		unsigned int fcr;				/* DLAB=0: FIFO control register, write-only */
		unsigned int afr;				/* DLAB=1: alternate function register */
	} w2;

	unsigned int lcr;					/* 0xF2_*0000C line control-register, read-write */
	unsigned int rvs0;					/* 0xF2_*00010 register rvs 4 byte */
	unsigned int lsr;					/* 0xF2_*00014 line status register, read-only/clear */
	unsigned int rvs1;					/* 0xF2_*00018 register, rvs 4 byte */
	unsigned int sr;					/* 0xF2_*0001C scratch -regsiter, write-only */
	unsigned int rvs2[0x3A0];			/* 0xF2_*000*** register, rvs 928 byte*/
	unsigned int tcnt;					/* 0xF2_*003C0 tx fifo cnt-register, read-write */
	unsigned int rcnt;					/* 0xF2_*003C4 rx fifo cnt-regsiter, read-write */
} UART_DEV_T;

#define UART_BASE	(UART_DEV_T*)REG_BASE_UART(0)		/* uart地址 */
#define UART_BASE1	(UART_DEV_T*)REG_BASE_UART(1)
#define UART_BASE2	(UART_DEV_T*)REG_BASE_UART(2)

#define UART0		UART_BASE
#define UART1		UART_BASE1
#define UART2		UART_BASE2

#define _RXB(u)		((u)->w0.rxb)
#define _TXB(u)		((u)->w0.txb)
#define _DLL(u)		((u)->w0.dll)
#define _IER(u)		((u)->w1.ier)
#define _DLM(u)		((u)->w1.dlm)
#define _IIR(u)		((u)->w2.iir)
#define _FCR(u)		((u)->w2.fcr)
#define _AFR(u)		((u)->w2.afr)
#define _LCR(u)		((u)->lcr)
#define _MCR(u)		((u)->mcr)
#define _LSR(u)		((u)->lsr)
#define _MSR(u)		((u)->msr)
#define _SCR(u)		((u)->scr)

#define _enable_rxirq(chn)		(WR_UART_IER(IER_RDA_IE | IER_RTO_IE, chn))

/* IER bits */
#define IER_RDA_INT_ENABLE		0x01			/* Received data available Interrup enable */
#define IER_THRE_INT_ENABLE		0x02			/* Transmitter holding register empty Interrup enable */
#define IER_RXLS_INT_ENABLE		0x04			/* Receiver line status register change Interrupt enable */
#define IER_MSR_INT_ENABLE		0x08			/* Modem status register change Interrupt enable */
#define IER_RTO_INT_ENABLE		0x10			/* Recieve fifo non-empty timeout Interrupt enable */
#define IER_RTI_INT_ENABLE		0x20			/* Recieve fifo counter timeout Interrupt enable */

#define IER_RDA_IE				0x01			/* Received data available Interrup enable */
#define IER_THRE_IE				0x02			/* Transmitter holding register empty Interrup enable */
#define IER_RXLS_IE				0x04			/* Receiver line status register change Interrupt enable */
#define IER_MSR_IE				0x08			/* Modem status register change Interrupt enable */
#define IER_RTO_IE				0x10			/* Recieve fifo non-empty timeout Interrupt enable */
#define IER_RTI_IE				0x20			/* Recieve fifo counter timeout Interrupt enable */

#define IER_RDA_ID				0x00			/* Received data available Interrup enable */
#define IER_THRE_ID				0x00			/* Transmitter holding register empty Interrup enable */
#define IER_RXLS_ID				0x00			/* Receiver line status register change Interrupt enable */
#define IER_MSR_ID				0x00			/* Modem status register change Interrupt enable */
#define IER_RTO_ID				0x00			/* Recieve fifo non-empty timeout Interrupt enable */
#define IER_RTI_ID				0x00			/* Recieve fifo counter timeout Interrupt enable */

/* FCR bits */
#define UART_FCR				0x0F			/* mask FIFO control register */
#define FCR_FIFO_ENABLE			0x01			/* fifo enable */
#define FCR_RX_FIFO_RESET		0x02			/* clear the rx fifo and reset the logic */
#define FCR_TX_FIFO_RESET		0x04			/* clear the tx fifo and reset the logic */
#define FCR_DMA_MODE_SELECT		0x08			/* dma模式选择 */
#define FCR_RX_TRIGGER_LSB		0x40			/* rx fifo interrupt trigger level */
#define FCR_RX_TRIGGER_MSB		0x80			/* rx fifo interrupt trigger level */

/* IIR bits */
#define IIR_MASK				0x0E			/* uart interrupt enable */
#define IIR_RX_STAT				0x06			/* 1 - receiver line status */
#define IIR_RX_PEND				0x04			/* 2 - receiver data available */
#define IIR_RX_TMOUT			0x0C			/* 2 - receiver timeout trigger */
#define IIR_TX_EMPTY			0x02			/* 3 - transmitter holding register empty */
#define IIR_TX_MODEM			0x00			/* 4 - transmitter holding register empty */

/* LCR bits */
#define WORD_LENGTH(n)			(((n)-5)&0x3)	/* select number of bits in each character */
#define LCR_STOP_BIT_ENABLE		0x04			/* specify the number of generated stop bits */
#define LCR_PARITY_ENABLE		0x08			/* parity enable */
#define LCR_EVEN_PARITY			0x10			/* even parity enable */
#define LCR_FORCE_PARITY		0x20			/* stick parity */
#define LCR_XMIT_BREAK			0x40			/* break control */
#define LCR_DLAB_ENABLE			0x80			/* divisor latch access */

/* LSR Bits */
#define LSR_RX_DATA_READY		0x01			/* Receive Data Ready(RDR) indicator */
#define LSR_OVERRUN_ERROR		0x02			/* Overrun Error(OE) indicator */
#define LSR_PARITY_ERROR		0x04			/* Parity Error(PE) indicator */
#define LSR_FRAMING_ERROR		0x08			/* Framing Error(FE) indicator */
#define LSR_BREAK_INTERRUPT		0x10			/* Break Interrupt(BI) indicator */
#define LSR_THRE				0x20			/* Transmit Holding Register Empty ( Transmit FIFO is empty) */
#define LSR_TEMP				0x40			/* Transmitter Empty indicator */
#define LSR_RXFE				0x80			/* At least one parity error, Rx FIFO error */

#define RX_READY(u)		(_LSR(u)&LSR_RX_DATA_READY)	/* 检测是否接收准备好 */
#define TX_READY(u)		(_LSR(u)&LSR_THRE)			/* 检测是否发送准备好 */

/* 各种波特率的等待超时，目前os-tick最短时间单位10ms */
#define URT_TMOUT_115200		2				/* 115200设置为1有问题 */
#define URT_TMOUT_9600			2
#define URT_TMOUT_4800			2
#define URT_TMOUT_2400			3
#define URT_TMOUT_1200			6
#define URT_CHN_OVTM			URT_TMOUT_1200

#define UART_CHN_NUM			3

#define UART_CHN_0				0
#define UART_CHN_1				1
#define UART_CHN_2				2
#define UART_CHN_DBG			UART_CHN_0
#define UART_CHN_DM				UART_CHN_1

#define BAUD_1200				1200
#define BAUD_2400				2400
#define BAUD_4800				4800
#define BAUD_9600				9600
#define BAUD_19200				19200
#define BAUD_115200				115200

#define PARITY_NONE				0x00
#define PARITY_EVEN				0x01
#define PARITY_ODD				0x02

#define STOP_ONE				0x00			/* stop bit - 1 */
#define STOP_ONE_HALF			0x01			/* stop bit - 1.5 */
#define STOP_TWO				0x02			/* stop bit - 2 */

#define UART_TX_UNIT			200				/* 发送单元，建议为10的整数倍 */
#define UART_RX_UNIT			0x80			/* 接收单元，必须为16的整数倍 */
//#define UART_RX_UNIT			0x00			/* 接收单元，必须为16的整数倍 */

//#define UART_BUF_SIZE			512				/* uart buf size */
#define UART_BUF_SIZE			(2048 + 256)	/* uart buf size */

#define UART_TMOUT_BCNT			5				/* 帧间隔为5字节超时 */
#define UART_BITS_PERBYTE		(8 + 2)			/* 串口每个字节发送bits数 8数据位 + 1停止位 + 1起始位 */
#define SYS_TMR_UNIT			10				/* 系统定时器的定时精度为10ms */
#define UART_CLK				(PLL_CLK / (GET_GBC_AHB_CLK_DIV() + 1))

typedef struct _UART_CFG
{
	unsigned int	baud;
	unsigned char	byte_bit;
	unsigned char	stop_bit;
	unsigned char	parity;
} uart_cfg_t;

typedef struct _uart_buffer{
	unsigned char	buf[0x200];
	unsigned int	last_tick;
	unsigned int	tick;
	unsigned int	head;
	unsigned int	len;
} uart_buffer_t;

typedef void (*tmout_callback)(estimer_t tmr, void *arg);

typedef struct
{
	unsigned int		rxlen;
	unsigned char		rxtmout;
	unsigned char		rxbuf[UART_BUF_SIZE];
	estimer_t			rx_tmr;
	tmout_callback		rx_tmout_cback;

	unsigned int		txlen;
	unsigned int		txpos;
	unsigned char		txbuf[UART_BUF_SIZE];
	unsigned char		txing;
	estimer_t			*tx_tmr;
	char				*uart_tmrname;
	tmout_callback		tx_tmout_cback;

	uart_cfg_t			cfg;					/* 测试接口，正式版可以去掉 */
} UART_INFO;

void esuart_init(void);

void esuart_config(uint32_t chn, uint32_t baud, uint32_t parity);

__isr__ void isr_esuart(void *data, void *context);

void esuart_remap(uint32_t chn);

void esuart_tick_hook(uint32_t chn);

void esuart_board_putchar(char ch, uint32_t chn);

u32 esuart_peak_data(uint32_t chn, uint8_t *buf, uint32_t len);
u32 esuart_read_data(uint32_t chn, uint8_t *buf, uint32_t len);

uint32_t esuart_send_data(uint32_t chn, uint8_t *buf, uint32_t len);

void uart_irq_enable(uint32_t chn);

void uart_irq_disable(uint32_t chn);

#endif	/* end of _UART_H */

