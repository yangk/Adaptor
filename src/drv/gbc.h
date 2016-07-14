/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:		gbc.h
Description:	GBC control
Author:			ch
Version:		v1.0
Date:			2015/06/24
History:
*/
/************************************************************************************************/

#ifndef _GBC_H_
#define _GBC_H_
/* GBC SOFT RESET */
#define SRST_TICK			(1 << 21)
#define SRST_GE_CG			(1 << 20)
#define SRST_GE_MRX			(1 << 19)
#define SRST_GE_MTX			(1 << 18)
#define SRST_MON			(1 << 17)
#define SRST_GE_ACU			(1 << 16)
#define SRST_UART2			(1 << 15)
#define SRST_UART1			(1 << 14)
#define SRST_UART0			(1 << 13)
#define SRST_GPIO			(1 << 12)
#define SRST_I2C			(1 << 11)
#define SRST_WDT			(1 << 10)
#define SRST_MDIO			(1 << 9)
#define SRST_SPI			(1 << 8)
#define SRST_GE				(1 << 7)
#define SRST_PMA_RX			(1 << 6)
#define SRST_PMA_TX			(1 << 5)
#define SRST_PMA			(1 << 4)
#define SRST_PMD			(1 << 3)
#define SRST_AIF			(1 << 2)
#define SRST_MC				(1 << 1)
#define SRST_SDMA			(1 << 0)

typedef enum {
	IRQ_CPU_ESTMR		= 5,
	IRQ_CPU_UART0		= 4,
	IRQ_CPU_UART1		= 4,
	IRQ_CPU_UART2		= 4,
} cpu_irq_line_enum_t;

#endif
