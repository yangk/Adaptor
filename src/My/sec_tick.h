#ifndef _SYS_TICK_H_
#define _SYS_TICK_H_

#include <stdint.h>


#define SEC_HZ	        20
#define TICKS_PER_SEC   (1000/SEC_HZ)

extern volatile uint32_t sec_jiffies;

int sec_tick_init(void);

#endif
