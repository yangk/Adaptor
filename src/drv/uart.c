/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		uart.c
Description:	uart control
Author:			ch
Version:		v1.0
Date:			2015/06/24
History:
*/
/************************************************************************************************/

#include "include.h"
#include "lightelf_uart_reg.h"
#include "lightelf_gbc_reg.h"
#include "lightelf_gpio_reg.h"

#include "sdk_svc.h"
#include "irq.h"
#include "gbc.h"
#include "gpio.h"
#include "uart.h"
#include "comfunc.h"
#include "sec_printf.h"

static uint32_t uart_chan[UART_CHN_NUM] = { UART_CHN_0, UART_CHN_1, UART_CHN_2};

static UART_INFO uart_info[UART_CHN_NUM];

static __tmr__ void esuart_rx_tmout(estimer_t tmr, void *arg);

static __tmr__ void esuart_tx_tmout(estimer_t tmr, void *arg);

static uint32_t esuart_rx_bytes(uint32_t chn, uint8_t *data, uint32_t len, uint32_t end);

static void isr_esuart_tx_byte(uint32_t chn);

/*****************************************************************************************************/
/*
	Function	: uart_init
	Description	: 串口初始化
	Input		:
	Output		:
	Return		:
	Notes		: uart0 已经被cmd模块占用，uart0的配置库里已经做好
*/
/*****************************************************************************************************/
void esuart_init(void)
{
	esuart_remap(UART_CHN_1);

	memset((uint8_t *)&uart_info, 0x00, sizeof(uart_info));

	uart_info[1].rx_tmout_cback		= esuart_rx_tmout;
	uart_info[1].tx_tmout_cback		= esuart_tx_tmout;

	uart_info[1].uart_tmrname		= "uart1_tmr";
	uart_info[1].rx_tmr = timer_create(1000,
										 1000,
										 TMR_TRIGGER,
										 uart_info[1].rx_tmout_cback,
										 &uart_chan[1],
										 uart_info[1].uart_tmrname);
	uart_info[1].tx_tmr = timer_create(1000,
										 1000,
										 TMR_TRIGGER,
										 uart_info[1].tx_tmout_cback,
										 &uart_chan[1],
										 uart_info[1].uart_tmrname);

	if(NULL == uart_info[1].rx_tmr)
	{
		esprintf("uart1 rxtimer init failed\n");
	}

	if(NULL == uart_info[1].tx_tmr)
	{
		esprintf("uart1 txtimer init failed\n");
	}

	esuart_config(UART_CHN_1, BAUD_115200, PARITY_EVEN);

	release_irq(IRQ_UART1);
	request_irq(IRQ_CPU_UART1, IRQ_UART1, isr_esuart, &uart_chan[1]);		/* uart1 */
}

/*****************************************************************************************************/
/*
	Function	: uart_cofig
	Description	: 串口初始化
	Input		:
	Output		:
	Return		:
	Notes		: chn = 1可能不能中断 AL_URT_CHN DEBUG_URT_CHN
*/
/*****************************************************************************************************/
void esuart_config(uint32_t chn, uint32_t baud, uint32_t parity)
{
	UART_DEV_T *uart = (UART_DEV_T * )REG_BASE_UART(chn);
	int divisor;
	cpu_sr_t cpu_sr;

	cpu_sr = ENTER_CRITICAL();

//	SET_GBC_DEBUG_MODE(0);							/* FPGA有用 */

	uart_info[chn].cfg.baud			= baud;
	uart_info[chn].cfg.byte_bit		= 8;
	uart_info[chn].cfg.parity		= parity;
	uart_info[chn].cfg.stop_bit		= 1;

	/* 设置baudrate */
	divisor = UART_CLK / ( baud * 16);				/* CPU clkDèòa×?×???μ?è·?¨￡? */

	_FCR(uart) = UART_FCR;							/* 清除RX/TX fifo */

	SET_UART_LC_DL(1, chn);							/* 打开锁存寄存器 */
	_DLM(uart) = (INT8U)(divisor >> 8);				/* 设置divisor */
	_DLL(uart) = (INT8U)(divisor & 0xFF);
	SET_UART_LC_DL(0, chn);							/* 关闭锁存寄存器 */

	SET_UART_LC_BITS(WORD_LENGTH(8), chn);			/* 设置数据位——8 */
	SET_UART_LC_SB(0, chn);							/* 停止位-1 */

	switch(parity)									/* 设置parity */
	{
	case PARITY_NONE:
		SET_UART_LC_PE(0, chn);						/* none Parity */
		break;

	case PARITY_EVEN:
		SET_UART_LC_PE(1, chn);
		SET_UART_LC_EP(1, chn);						/* Even Parity select */
		SET_UART_LC_SP(0, chn);						/* mark Parity */
		break;

	case PARITY_ODD:
		SET_UART_LC_PE(1, chn);
		SET_UART_LC_EP(0, chn);						/* Odd Parity sslect */
		SET_UART_LC_SP(0, chn);						/* mark Parity */
		break;

	default:
		SET_UART_LC_PE(0, chn);						/* none Parity */
		break;
	}

	_FCR(uart) = UART_FCR;							/* 清除RX/TX fifo */

	SET_UART_LC_DL(1, chn);							/* 打开锁存寄存器 */
	SET_UART_IIFT(UART_CLK / 1000000 - 1, chn);		/* 打开锁存设置1us/ticks */
	SET_UART_LC_DL(0, chn);							/* 关闭锁存寄存器 */
	SET_UART_IIFC(UART_RX_UNIT, chn);				/* 128字节中断一次 */
	SET_UART_IIFT(1000000 * 5 * 10 / baud, chn);	/* 5字节非空超时 单位为us*/

	_enable_rxirq(chn);								/* 开UART接收中断 */

	EXIT_CRITICAL(cpu_sr);
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 串口中断
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
__isr__ void isr_esuart(void *data, void *context)
{
	uint32_t chan, len = 0, rxlen, iifc;
	uint8_t buf[256];
	u32 lsr;

	chan = *(uint32_t * )data;

	iifc = GET_UART_IIFC(chan) & IIR_MASK;

	lsr = RD_UART_LSR(chan);
	if (lsr & 0x100)
	{
		while(lsr & 0x01)
		{
			buf[len++] = RD_UART_BUFR(chan);
			lsr = RD_UART_LSR(chan);
		}
		esuart_rx_bytes(chan, buf, len, 1);
	}

	if (iifc == IIR_RX_PEND)
	{
		lsr = RD_UART_LSR(chan);
		rxlen = (UART_RX_UNIT == 0) ? 1 : UART_RX_UNIT - 5;		/* IIFC */

		for(len = 0; len < rxlen; len++)
		{
			if (GET_UART_LS_DR(chan))
			{
				buf[len] = RD_UART_BUFR(chan);
			}
		}

		esuart_rx_bytes(chan, buf, len, 0);
	}

	if(iifc == IIR_TX_EMPTY)
	{
		isr_esuart_tx_byte(chan);
	}
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 串口映射到GPIO
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void esuart_remap(uint32_t chn)
{
	RST_GPIO_GPIO_GE_MII_EN();			/* 初始化GE_MII??? */
	switch(chn)
	{
	case UART_CHN_1:
		SET_GPIO_GPIO_URX1_MAP(GPIO_UART1RX);
		SET_GPIO_GPIO_UTX1_MAP(GPIO_UART1TX);
		SET_GPIO_GPIO_UART1_EN(1);

		SET_GBC_RST_EN(SRST_UART1);
		SET_GBC_RST_EN(0);

		break;

	default:
		break;
	}
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 串口字节超时中断回调函数
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
static __tmr__ void esuart_rx_tmout(estimer_t tmr, void *arg)
{
	uint32_t chan;

	chan = *(uint32_t * )arg;

	esuart_tick_hook(chan);
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 发送字节超时
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
static __tmr__ void esuart_tx_tmout(estimer_t tmr, void *arg)
{
	uint32_t chan;

	chan = *(uint32_t * )arg;

	if (uart_info[chan].txlen > 0)
	{
		isr_esuart_tx_byte(chan);
		return;
	}

	uart_info[chan].txlen	= 0;
	uart_info[chan].txing	= FALSE;
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 串口中断
	Input		:
	Output		:
	Return		:
	Notes		: 防护定时器的定时长度为：最后一个码片的接收时间+5字节超时时间
*/
/*****************************************************************************************************/
static uint32_t esuart_rx_bytes(uint32_t chn, uint8_t *data, uint32_t len, uint32_t end)
{
	uint32_t tmout;

	if(uart_info[chn].rx_tmr != NULL)
	{
		timer_stop(uart_info[chn].rx_tmr, TMR_TRIGGER);
	}

	if(end == 0x00)
	{
		tmout = (UART_RX_UNIT + UART_TMOUT_BCNT) * UART_BITS_PERBYTE
				/ uart_info[chn].cfg.baud * 1000 / SYS_TMR_UNIT + 5;

		timer_modify(uart_info[chn].rx_tmr,
					tmout,
					tmout,
					TMR_TRIGGER,
					uart_info[chn].rx_tmout_cback,
					&uart_chan[chn],
					uart_info[chn].uart_tmrname);

		timer_start(uart_info[chn].rx_tmr);
	}
	else											/* 帧接收完成 */
	{
		esuart_tick_hook(chn);						/* post在中断内完成 */
	}

	if(len > UART_BUF_SIZE)							/* 过大，不能处理 */
	{
		return 0;
	}

	if(uart_info[chn].rxlen + len > UART_BUF_SIZE)	/* 溢出，循环覆盖 */
	{
		uart_info[chn].rxlen = 0;
	}

	memcpy(uart_info[chn].rxbuf + uart_info[chn].rxlen, data, len);
	uart_info[chn].rxlen += len;

	return len;
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 串口中断
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void esuart_tick_hook(uint32_t chn)
{
	if (uart_info[chn].rx_tmr != NULL)
	{
		timer_stop(uart_info[chn].rx_tmr, TMR_TRIGGER);
	}

	uart_info[chn].rxtmout = TRUE;
}

/*****************************************************************************************************/
/*
	Function	: isr_uart
	Description	: 串口中断
	Input		:
	Output		:
	Return		:
	Notes		: 多字节单元发送时可以考虑可以用DMA搬移，然后等待发送完成？？？
*/
/*****************************************************************************************************/
static void isr_esuart_tx_byte(uint32_t chn)
{
	uint32_t i, txlen, tmout;

	if(uart_info[chn].tx_tmr != NULL)
	{
		timer_stop(uart_info[chn].tx_tmr, TMR_TRIGGER);
	}

	if(FALSE == uart_info[chn].txing)						/* 本次中断检测模块非发送态 */
	{
		uart_info[chn].txlen = 0;
	}

	if(uart_info[chn].txlen > UART_TX_UNIT)					/* 确定发送块的大小 */
	{
		txlen = UART_TX_UNIT;
	}
	else if (uart_info[chn].txlen > 0)
	{
		txlen = uart_info[chn].txlen;						/*  */
	}
	else if (uart_info[chn].txlen == 0)
	{
		SET_UART_IE_THRE(0, chn);							/* 最后一块发送完毕，关中断 */
		uart_info[chn].txing = FALSE;
		return;
	}

	uart_info[chn].txlen -= txlen;

	tmout = (txlen + UART_TMOUT_BCNT) * UART_BITS_PERBYTE * 1000
			/ uart_info[chn].cfg.baud / SYS_TMR_UNIT + 1;			/* 防护定时器的定时长度 */

	for(i = 0; i < txlen; i++)
	{
		SET_UART_FIFO(uart_info[chn].txbuf[uart_info[chn].txpos++], chn);
	}

	timer_modify(uart_info[chn].tx_tmr,
				tmout,
				tmout,
				TMR_TRIGGER,
				uart_info[chn].tx_tmout_cback,
				&uart_chan[chn],
				uart_info[chn].uart_tmrname);

	timer_start(uart_info[chn].tx_tmr);
}

/*****************************************************************************************************/
/*
	Function	: dev_uart_init
	Description	: 单字节发送
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void esuart_board_putchar(char ch, uint32_t chn)
{
	/* 须先给发送缓冲区赋值，再等空，保证最后一个字节发完 */
	SET_UART_FIFO(ch, chn);
	while(GET_UART_LS_TFE(chn) == 0);	/* 发送等标志会清掉接收的RTO，所以要么采用块发送，要么在处理完成后保留标志，判断是否曾经产生过RTO */
	return;
}

/*****************************************************************************************************/
/*
	Function	: uart_read_data
	Description	: 串口接收对外接口
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
u32 esuart_read_data(uint32_t chn, uint8_t *buf, uint32_t len)
{
	if(FALSE == uart_info[chn].rxtmout || (0 == uart_info[chn].rxlen))
	{
		return 0;							/* 串口正在接收数据或缓冲区无数据 */
	}

	uart_irq_disable(chn);					/* 关闭接收发送中断 */

	if (uart_info[chn].rxlen < len)
	{
		len = uart_info[chn].rxlen;
	}

	memcpy(buf, uart_info[chn].rxbuf, len);

	uart_info[chn].rxlen		= 0;		/* 清空rx缓冲区 */
	uart_info[chn].rxtmout		= FALSE;

	uart_irq_enable(chn);					/* 开接收中断 */

	return len;
}
u32 esuart_peak_data(uint32_t chn, uint8_t *buf, uint32_t len)
{
	if (FALSE == uart_info[chn].rxtmout || (0 == uart_info[chn].rxlen))
	{
		return 0;							/* 串口正在接收数据或缓冲区无数据 */
	}

	uart_irq_disable(chn);					/* 关闭接收发送中断 */
    len = min(len, uart_info[chn].rxlen);
	uart_irq_enable(chn);					/* 开接收中断 */

	memcpy(buf, uart_info[chn].rxbuf, len);

	return len;
}

/*****************************************************************************************************/
/*
	Function	: uart_send_data
	Description	: 串口发送对外接口
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
uint32_t esuart_send_data(uint32_t chn, uint8_t *buf, uint32_t len)
{
	if((len > UART_BUF_SIZE) || (len == 0))
	{
		return 0;
	}

    pr_info("uart1: send data, len[%d]\n", len);
    print_debug_array(KERN_INFO, buf, len);
#if 1
	memcpy(uart_info[chn].txbuf, buf, len);
	uart_info[chn].txlen = len;
	uart_info[chn].txpos = 0;

	uart_info[chn].txing = TRUE;
	isr_esuart_tx_byte(chn);
	SET_UART_IE_THRE(1, chn);			/* 需要在写入fifo数据以后开启中断 */

#else
	u32 i;
	for (i = 0; i < len; i++)
	{
		esuart_board_putchar(buf[i], chn);	/* RC 中断标志位RTO，硬件无法再回复标志，会导致硬件的终端入口 */
	}

#endif
	return len;
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void uart_irq_enable(uint32_t chn)
{
	cpu_sr_t cpu_sr;

	cpu_sr = ENTER_CRITICAL();

	switch(chn)
	{
	case UART_CHN_1:
		SET_GBC_CIE(GET_GBC_CIE() | (1 << IRQ_CPU_UART1));		/* enable cpu interrupt - 5 */

		break;

	case UART_CHN_2:
		SET_GBC_CIE(GET_GBC_CIE() | (1 << IRQ_CPU_UART1));		/* enable cpu interrupt - 5 */

		break;

	default:

		break;
	}

	EXIT_CRITICAL(cpu_sr);
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void uart_irq_disable(uint32_t chn)
{
	cpu_sr_t cpu_sr;

	cpu_sr = ENTER_CRITICAL();

	switch(chn)
	{
	case UART_CHN_1:
		SET_GBC_CIE(GET_GBC_CIE() & (~(1 << IRQ_CPU_UART1)));		/* disable cpu interrupt - 5 */

		break;

	case UART_CHN_2:
		SET_GBC_CIE(GET_GBC_CIE() & (~(1 << IRQ_CPU_UART1)));		/* disable cpu interrupt - 5 */

		break;

	default:

		break;
	}

	EXIT_CRITICAL(cpu_sr);
}
