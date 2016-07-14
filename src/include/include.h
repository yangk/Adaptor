#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <xtensa/xtruntime.h>

#include "types.h"
#include "cpu.h"
#include "sec_printf.h"

extern void (*esprintf)(const char *fmt, ...);

#endif
