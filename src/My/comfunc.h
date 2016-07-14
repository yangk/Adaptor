#ifndef _COMFUNC_H_
#define _COMFUNC_H_

//#include "estype.h"

#include <stdint.h>

#define SIZE_1K 1024L
#define SIZE_1M (SIZE_1K*SIZE_1K)

#define set_bit(x, bit) ((x) |= 1 << (bit))
#define clr_bit(x, bit) ((x) &= ~(1 << (bit)))
#define tst_bit(x, bit) ((x) & (1 << (bit)))
#define get_bits(val,x1,x2) (((val)>>(x1))&((1<<((x2)-(x1)+1))-1))

#define min(a, b) ((a)<(b) ? (a):(b))
#define max(a, b) ((a)>(b) ? (a):(b))

#define array_size(array) (sizeof(array)/sizeof(*array))

#define _swab16(x) ((uint16_t)(				\
    (((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |			\
    (((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define _swab32(x) ((uint32_t)(				\
    (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |		\
    (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |		\
    (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |		\
    (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#ifdef BIG_ENDIAN
#define cpu_to_le32(x) ((uint32_t)_swab32((x)))
#define le32_to_cpu(x) ((uint32_t)_swab32((uint32_t)(x)))
#define cpu_to_le16(x) ((uint16_t)_swab16((x)))
#define le16_to_cpu(x) ((uint16_t)_swab16((uint16_t)(x)))
#define cpu_to_be32(x) ((uint32_t)(x))
#define be32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_be16(x) ((uint16_t)(x))
#define be16_to_cpu(x) ((uint16_t)(x))
#else
#define cpu_to_le32(x) ((uint32_t)(x))
#define le32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_le16(x) ((uint16_t)(x))
#define le16_to_cpu(x) ((uint16_t)(x))
#define cpu_to_be32(x) ((uint32_t)_swab32((x)))
#define be32_to_cpu(x) ((uint32_t)_swab32((uint32_t)(x)))
#define cpu_to_be16(x) ((uint16_t)_swab16((x)))
#define be16_to_cpu(x) ((uint16_t)_swab16((uint16_t)(x)))
#endif

#define bcd2bin(val) (((val) & 0x0f) + ((val) >> 4) * 10)
#define bin2bcd(val) ((((val) / 10) << 4) + (val) % 10)

#define swap(a, b) do {a ^= b; b ^= a; a ^= b;} while (0)

uint8_t checksum(const void *data, int len);

/* byte order */
uint32_t get_le_val(const uint8_t * p, int bytes);
uint32_t get_be_val(const uint8_t * p, int bytes);
void put_le_val(uint32_t val, uint8_t * p, int bytes);
void put_be_val(uint32_t val, uint8_t * p, int bytes);

int split(char *str, char **arr, const char *del);
int is_all_xx(const void *s1, uint8_t val, int n);

const char *arr2str(const void *arr, int len, void *dest, int maxlen);
int str2arr(const char *str, void *out, int maxlen);
char *i2str(uint8_t val, char *destStr);
void print_debug_array(const char *lvl, const void *arr, int len);

void reverse(void *buff, size_t len);

uint16_t calc_crc16(uint16_t crc, const void *buf, int size);
#endif
