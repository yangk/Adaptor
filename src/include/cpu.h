/*
 * Copyright: (c) 2006-2007, 2008 Triductor Technology, Inc.
 * All Rights Reserved.
 *
 * File:        sys.c
 * Purpose:     System level functions.
 * History:     01/15/2007, created by andyzhou
 */

#ifndef __CPU_H__
#define __CPU_H__


#include "types.h"
#include <xtensa/hal.h>
#include <xtensa/tie/xt_core.h>
#include <xtensa/config/specreg.h>
#include <xtensa/config/core.h>
#include <xtensa/xtruntime.h>


#if defined(ISS_VERSION) || defined(XTMP_VERSION)
#include <xtensa/simcall.h>
#endif

#if defined(FPGA_VERSION)
#define PLL_CLK                 (50000000)
#elif defined(ASIC_VERSION)
#define PLL_CLK                 (400000000)
#else
#error  "Build Properties, Missing ASIC_VERSION or FPGA_VERSION ..."
#endif

extern uint32_t CPU_CLK;

#define FRAME_SIZE		124
#define FRAME_AR(x)		(x * 4)
#define FRAME_LBEG		64
#define FRAME_LEND		68
#define FRAME_LCOUNT		72
#define FRAME_SAR		76
#define FRAME_PS		80
#define FRAME_PC		84
#define CACHE_LINE_SIZE		32

#define CPU_CYCLE_PER_US	(CPU_CLK / 1000000)
#define CPU_CYCLE_PER_MS	(CPU_CLK / 1000)
#define OS_TICKS_PER_SEC	100
#define MS_PER_OS_TICKS		(1000 / OS_TICKS_PER_SEC)

#define TIMER0_INTERVAL		(CPU_CLK / OS_TICKS_PER_SEC)

#define TIMER0_INT_MASK		(1 << XCHAL_TIMER0_INTERRUPT)
#define TIMER1_INT_MASK		(1 << XCHAL_TIMER1_INTERRUPT)
#define TIMER2_INT_MASK		(1 << XCHAL_TIMER2_INTERRUPT)

#define SOFTWARE0_INT_MASK	0x0080
#define SOFTWARE1_INT_MASK	0x0800

#define CRITICAL_INFO_FLAG	0x378B6F29
//#define CRITICAL_DEBUG
#ifdef CRITICAL_DEBUG
extern uint32_t sys_lock(const char *func, uint32_t line);
extern void sys_unlock(uint32_t cpu_sr, const char *func, uint32_t line);
#define ENTER_CRITICAL() sys_lock(__func__, __LINE__)
#define EXIT_CRITICAL(cpu_sr) sys_unlock(cpu_sr, __func__, __LINE__)
#else
#define ENTER_CRITICAL()	\
({				\
	uint32_t tmp;		\
				\
	tmp = XT_RSIL(5);	\
	XT_ESYNC();		\
				\
	tmp;			\
})
#define EXIT_CRITICAL(cpu_sr)	do {	\
	XT_WSR_PS(cpu_sr);			\
	XT_ESYNC();				\
} while(0)
#endif

//extern void OSIntExit(int32_t arg) __attribute__ ((section(".iram0.text")));
//extern void OSCtxSwIntsEnable(void);


#define irq_idx_to_level(idx)	(((idx) < 8)  ? 1 :	\
				 ((idx) < 9)  ? 2 :	\
				 ((idx) < 12) ? 3 :	\
				 ((idx) < 13) ? 4 : 5)

#define irq_nest_allow_level(level) _xtos_set_intlevel((level) != 1 ? (level) : 0)
#define irq_nest_restore_level(level) _xtos_set_intlevel(level)


typedef int32_t	atomic_t;

/* atomic_set - Set atomic variable
 */
static __inline__ void atomic_set(atomic_t *a, uint32_t i)
{
	uint32_t cpu_sr;

	cpu_sr = ENTER_CRITICAL();
	*a = i;
	EXIT_CRITICAL(cpu_sr);
	return;
}
/* atomic_inc - Increment atomic variable
 */
static __inline__ atomic_t atomic_inc(atomic_t *a)
{
	uint32_t cpu_sr;
	atomic_t old;

	cpu_sr = ENTER_CRITICAL();
	old = (*a)++;
	EXIT_CRITICAL(cpu_sr);

	return old;
}
/* atomic_dec - Decrement atomic variable
 */
static __inline__ atomic_t atomic_dec(atomic_t *a)
{
	uint32_t cpu_sr;
	atomic_t old;

	cpu_sr = ENTER_CRITICAL();
	old = (*a)--;
	EXIT_CRITICAL(cpu_sr);

	return old;
}
/* atomic_dec_and_test - Decrement and test, returns true if the result is 0,
 *	or false for all other cases.
 */
static __inline__ uint32_t atomic_dec_and_test(atomic_t *a)
{	
	uint32_t cpu_sr, ret;
	
	cpu_sr = ENTER_CRITICAL();
	(*a)--;
	ret = *a ? 0 : 1;
	EXIT_CRITICAL(cpu_sr);

	return ret;
}
/* atomic_test_zero - Test atomic variable whether zero or not.
 */
static __inline__ uint32_t atomic_test_zero(atomic_t *a)
{
	uint32_t cpu_sr, ret;

	cpu_sr = ENTER_CRITICAL();
	ret = *a;
	EXIT_CRITICAL(cpu_sr);
	
	return !ret;
}
/* atomic_test_zero - Test atomic variable whether non-zero or not.
 */
static __inline__ uint32_t atomic_test_true(atomic_t *a)
{
	uint32_t cpu_sr, ret;

	cpu_sr = ENTER_CRITICAL();
	ret = *a;
	EXIT_CRITICAL(cpu_sr);

	return ret;
}
/* atomic_set_bit - Set atomic variable's specific bit.
 */
static __inline__ void atomic_set_bit(uint8_t bit, atomic_t *val)
{
	uint32_t cpu_sr;
	
	cpu_sr = ENTER_CRITICAL();
	*val |=  1 << (bit);
	EXIT_CRITICAL(cpu_sr);

	return;
}
/* atomic_clear_bit - Clear atomic variable's specific bit.
 */
static __inline__ void atomic_clear_bit(uint8_t bit, atomic_t *val)
{
	uint32_t cpu_sr;
	
	cpu_sr = ENTER_CRITICAL();
	*val &= ~(1 << (bit));
	EXIT_CRITICAL(cpu_sr);

	return;
}
/* atomic_test_bit - Test atomic variable's specific bit whether
 *	non-zero or not.
 */
static __inline__ uint32_t atomic_test_bit(uint8_t bit, atomic_t *val)
{
	uint32_t cpu_sr, ret;
	
	cpu_sr = ENTER_CRITICAL();
	ret = *val & (1 << (bit));
	EXIT_CRITICAL(cpu_sr);

	return ret;
}


/*
 *      These inlines deal with timer wrapping correctly. You are
 *      strongly encouraged to use them
 *      1. Because people otherwise forget
 *      2. Because if the timer wrap changes in future you won't have to
 *         alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 *
 *	Note: a, b shall both be unsigned long integers. a shall not be
 * far too much from b.
 */
#ifndef time_after
#define time_after(a,b)		((long)(b) - (long)(a) < 0)
#endif

#ifndef time_before
#define time_before(a,b)	time_after(b,a)
#endif

#ifndef time_after_eq
#define time_after_eq(a,b)	((long)(a) - (long)(b) >= 0)
#endif

#ifndef time_before_eq
#define time_before_eq(a,b)	time_after_eq(b,a)
#endif

#ifndef time_diff
#define time_diff(a,b)		(unsigned long)((long)(a) - (long)(b))
#endif

#define DelayUs(us, cond)	do {					\
	INT32U ulCcountStart;						\
	INT32U ulCcountCur;						\
	INT32U diff;							\
									\
	ulCcountStart = xthal_get_ccount();				\
									\
	while (cond) {							\
		ulCcountCur = xthal_get_ccount();			\
		if(ulCcountCur < ulCcountStart)				\
			diff = ~(ulCcountStart - ulCcountCur);		\
		else							\
			diff = ulCcountCur - ulCcountStart;		\
									\
		if(diff >= (us) * CPU_CYCLE_PER_US)			\
			break;						\
	}								\
}while(0)

#endif /* __CPU_H__ */
