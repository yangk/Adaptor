/*
 * Copyright: (c) 2006-2007, 2008 Triductor Technology, Inc.
 * All Rights Reserved.
 *
 * File:        types.h
 * Purpose:     The primitive types for the whole project.
 * History:     10/15/2006, created by andyzhou
 */

#ifndef _TYPES_H
#define _TYPES_H

typedef enum
{
	false	= 0,
	true	= 1
} bool;

typedef unsigned char			BOOLEAN;
typedef unsigned char			INT8U;
typedef signed char				INT8S;
typedef unsigned short			INT16U;
typedef signed short			INT16S;
typedef unsigned int			INT32U;
typedef signed int				INT32S;
typedef float					FP32;
typedef double					FP64;

typedef long long				INT64S;
typedef unsigned long long		INT64U;

typedef unsigned long			ulong_t;
typedef unsigned int			uint32_t;
typedef unsigned short			uint16_t;
typedef unsigned char			uint8_t;

typedef long					long_t;
typedef int						int32_t;
typedef short					int16_t;
typedef signed char				int8_t;

typedef signed int				s32;
typedef signed short			s16;
typedef signed char				s8;

typedef long long				s64;
typedef signed int  const		sc32;						/* Read Only */
typedef signed short const		sc16;						/* Read Only */
typedef signed char  const		sc8;						/* Read Only */

typedef volatile signed int		vs32;
typedef volatile signed short	vs16;
typedef volatile signed char	vs8;

typedef volatile signed int  const	vsc32;					/* Read Only */
typedef volatile signed short const	vsc16;					/* Read Only */
typedef volatile signed char  const	vsc8;					/* Read Only */

typedef unsigned long long		u64;
typedef unsigned int			u32;
typedef unsigned short			u16;
typedef unsigned char			u8;

typedef u32						cpu_sr_t;

#define htons(n)		((uint16_t)(((n) & 0xff) << 8) | (((n) & 0xff00) >> 8))
#define htonl(n)		((uint32_t)( \
				(((uint32_t)(n) & (uint32_t)0x000000ffUL) << 24) | \
				(((uint32_t)(n) & (uint32_t)0x0000ff00UL) <<  8) | \
				(((uint32_t)(n) & (uint32_t)0x00ff0000UL) >>  8) | \
				(((uint32_t)(n) & (uint32_t)0xff000000UL) >> 24)))
#define ntohs(n)		htons(n)
#define ntohl(n)		htonl(n)

#define __isr__
#define __plc_wq__
#define __tmr__

#define DISPLAY_MSG		0

#define DISABLE			0
#define ENABLE			1

#define MAX(a,b)		((a) >= (b) ? (a) : (b))
#define MIN(a,b)		((a) <= (b) ? (a) : (b))
#define US_SWAP(a)		((a) = (INT16U)(((INT16U)(a) >> 8) | ((INT16U)(a) << 8)))
#define mod_ceiling(a, b)	(((a) + (b) - 1) / (b))
#define mod_floor(a, b)		((a) / (b))

#ifndef NULL
#define NULL			0
#endif

#ifndef TRUE
#define TRUE			(1 == 1)
#endif

#ifndef FALSE
#define FALSE			(!TRUE)
#endif

#ifndef OK
#define OK			0
#endif

#ifndef ERROR
#define ERROR			-1
#endif

static __inline__ void split_write_two(uint8_t *addr, uint16_t two, uint8_t endian)
{
	uint8_t byte0, byte1;
	
	byte0 = (two & 0xff);
	byte1 = (two & 0xff00) >> 8;

	if (endian) {
		*addr = byte1;
		*(addr+1) = byte0;
	} else {
		*addr = byte0;
		*(addr+1) = byte1;
	}
	
	return;
}

#define split_read_two(addr, endian)			\
	((endian) ?					\
	 (uint16_t)((*(addr) << 8) | (*(addr+1))) :	\
	 (uint16_t)((*(addr)) | (*(addr+1) << 8)))

static __inline__ void split_write_four(uint8_t *addr, uint32_t four, uint8_t endian)
{
	uint8_t byte0, byte1, byte2, byte3;
	
	byte0 = (four & 0xff);
	byte1 = (four & 0xff00) >> 8;
	byte2 = (four & 0xff0000) >> 16;
	byte3 = (four & 0xff000000) >> 24;

	if (endian) {
		*addr     = byte3;
		*(addr+1) = byte2;
		*(addr+2) = byte1;
		*(addr+3) = byte0;
	} else {
		*addr     = byte0;
		*(addr+1) = byte1;
		*(addr+2) = byte2;
		*(addr+3) = byte3;
	}
	return;
}


#define split_read_four(addr, endian)					\
	((endian) ?							\
	 (uint32_t)((*(addr) << 24) | (*(addr+1) << 16) | (*(addr+2) << 8) | (*(addr+3))) : \
	 (uint32_t)((*(addr)) | (*(addr+1) << 8) | (*(addr+2) << 16) | (*(addr+3) << 24)))


#endif	/* end of _TYPES_H */
