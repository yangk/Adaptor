#ifndef _TIMER_H_
#define _TIMER_H_

#define TMR_NULL		0
#define TMR_PERIODIC		(1 << 0)
#define TMR_TRIGGER		(1 << 1)
#define TMR_CALLBACK		(1 << 2)
#define TMR_REALTIME		(1 << 3)

#define TMR_STOPPED		0
#define TMR_RUNNING		1

typedef void * estimer_t;

extern estimer_t timer_create(u32 first,
								u32 interval,
								u32 opt,
								void (*fn)(estimer_t tmr, void *arg),
								void *arg,
								char *name);

extern int timer_delete(estimer_t tmr);

extern int timer_start(estimer_t tmr);

extern int timer_stop(estimer_t tmr, u32 opt);

extern int timer_modify(estimer_t tmr,
						u32 first,
						u32 interval,
						u32 opt,
						void (*fn)(estimer_t tmr, void *arg),
						void *arg,
						char *name);

extern int timer_query(estimer_t tmr);

extern int timer_get_ms(void);

extern void delay10ms(u32 time);

#endif
