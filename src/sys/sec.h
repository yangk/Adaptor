#ifndef _SEC_H_
#define _SEC_H_

#include "sdk.h"

#define MyStackSize			(1024 * 10)						//40K

#define START_INIT()		do{ if(sec_init(argc, argv) < 0) return 0; } while(0)

int sec_init(int argc, char *argv[]);

#endif
