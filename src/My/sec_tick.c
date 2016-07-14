#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "timer.h"
#include "comfunc.h"
#include "sec_tick.h"

//---------------------------------------------------------------------------------------
#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*SEC_HZ))
//---------------------------------------------------------------------------------------
volatile uint32_t sec_jiffies = INITIAL_JIFFIES;
//---------------------------------------------------------------------------------------
static void sec_tick_handle(estimer_t tmr, void *arg)
{
    sec_jiffies++;
}
//---------------------------------------------------------------------------------------
int sec_tick_init(void)
{
    estimer_t tick_tmr = timer_create(TICKS_PER_SEC / 10, TICKS_PER_SEC / 10, TMR_PERIODIC, 
        sec_tick_handle, NULL, "sec_tick_tmr");
    if (!tick_tmr)
    {
        esprintf("creat sec tick timer failed!\n");
        return -1;
    }
    timer_start(tick_tmr);
    return 0;
}
//---------------------------------------------------------------------------------------
