/*
 * Copyright: (c) 2006-2007, 2008 Triductor Technology, Inc.
 * All Rights Reserved.
 *
 * File:	irq.h
 * Purpose:	System-level interrupt request handling.
 * History:	11/07/2008, created by jetmotor
 */

#ifndef _IRQ_H
#define _IRQ_H


#include "types.h"
#include "list.h"


typedef void (*irq_handler_fp)(void *data, void *context);

#define NR_C_IRQ_MAX	15

typedef struct c_irq_s {
	uint32_t	idx;
	
	list_head_t	list;
	uint32_t	nr;
} c_irq_t;

enum {
	IRQ_PMD_PHY,
	IRQ_TICK,
	IRQ_PMD_ERR,
	IRQ_PMA_HDR,
	IRQ_PMA_ERR,
	IRQ_MC,
	IRQ_SPI,
	IRQ_GPIO_EXT,
	IRQ_UART0,
	IRQ_SDMA_CH0,
	IRQ_SDMA_CH1,
	IRQ_SDMA_CH2,
	IRQ_SDMA_CH3,
	IRQ_SDMA_CH4,
	IRQ_SDMA_CH5,
	IRQ_SDMA_CH6,
	IRQ_SDMA_CH7,
	IRQ_I2C,
	IRQ_GE,
	IRQ_MDIO_MGDI,
	IRQ_UART1,
	IRQ_UART2,
	IRQ_ACU,
	IRQ_PMA_PMI,
	IRQ_GE_GEI,
	IRQ_ESTMR,
	NR_M_IRQ_MAX
};


typedef struct m_irq_s {
	list_head_t	link;

	const char	*name;
	uint32_t	idx;
	uint32_t	mask;
	void		*data;
	void		*context;
	c_irq_t		*c_irq;

	void (*handler)(void *data, void *context);
} m_irq_t;

//extern c_irq_t C_IRQ[];
//extern m_irq_t M_IRQ[];


/* request_irq - Dispatch a MODULE irq line onto a CPU irq line. Both the MODULE
 * line and the CPU irq line are enbled.
 * @c_irq_idx: CPU irq line
 * @m_irq_idx: MODULE irq line
 * @handler:
 * @data:
 */
extern void request_irq(int32_t c_irq_idx, int32_t m_irq_idx, irq_handler_fp handler, void *data);
/* release_irq - Release a MODULE irq line from a CPU irq line. Both the MODULE
 * irq line and the CPU irq line (if none of MODULE irq lines are attached) will
 * be disabled.
 * @m_irq_idx: MODULE irq line
 */
extern void release_irq(int32_t m_irq_idx);
extern void enable_irq(int32_t m_irq_idx, void *context);
extern void disable_irq(int32_t m_irq_idx);


#endif	/* end of _IRQ_H */
