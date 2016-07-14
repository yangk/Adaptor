#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include.h"
#include "sec_printf.h"
#include "comfunc.h"

//---------------------------------------------------------------------------------------
uint8_t checksum(const void *data, int len)
{
    uint8_t cs = 0;

    while (len-- > 0)
        cs += *((uint8_t *) data + len);
    return cs;
}
//---------------------------------------------------------------------------------------
uint32_t get_le_val(const uint8_t * p, int bytes)
{
    uint32_t ret = 0;

    while (bytes-- > 0)
    {
        ret <<= 8;
        ret |= *(p + bytes);
    }
    return ret;
}
uint32_t get_be_val(const uint8_t * p, int bytes)
{
    uint32_t ret = 0;
    while (bytes-- > 0)
    {
        ret <<= 8;
        ret |= *p++;
    }

    return ret;
}
void put_le_val(uint32_t val, uint8_t * p, int bytes)
{
    while (bytes-- > 0)
    {
        *p++ = val & 0xFF;
        val >>= 8;
    }
}
void put_be_val(uint32_t val, uint8_t * p, int bytes)
{
    while (bytes-- > 0)
    {
        *(p + bytes) = val & 0xFF;
        val >>= 8;
    }
}
//---------------------------------------------------------------------------------------
int is_all_xx(const void *s1, uint8_t val, int n)
{
    while (n && *(uint8_t *) s1 == val)
    {
        s1 = (uint8_t *) s1 + 1;
        n--;
    }
    return !n;
}
//---------------------------------------------------------------------------------------
char *i2str(uint8_t val, char *dest)
{
    const char *charmap = "0123456789ABCDEF";

    *dest++ = charmap[get_bits(val, 4, 7)];
    *dest++ = charmap[get_bits(val, 0, 3)];
    *dest++ = '\0';
    return dest-3;
}
const char *arr2str(const void *arr, int len, void *dest, int maxlen)
{
    const uint8_t *_arr = (const uint8_t *)arr;
    char *_dest = (char *)dest;

    while (len-- && maxlen > 0)
    {
        i2str(*_arr++, _dest);
        _dest += 2;
        *_dest++ = ' ';
        maxlen -= 3;
    }
    if (_dest - (char*)dest > 0) *--_dest = '\0';

    return (const char *)dest;
}

void print_debug_array(const char *lvl, const void *arr, int len)
{
    char str[100];
    int idx = 0;

    while (len > 0)
    {
        int _len = min(len, 16);

        strcpy(str, lvl);
        arr2str(arr+idx, _len, &str[2], sizeof(str)-2);
        sec_printf(str);
        strcpy(str, lvl);
        strcpy(&str[2], "\n");
        sec_printf(str);
        len -= _len;
        idx += _len;
    }
}
//---------------------------------------------------------------------------------------
int split(char *str, char **arr, const char *del)
{
    if (!str) return 0;

    char **src = arr;

    for (*arr++ = str; *str; str++)
    {
        if (*str != *del) continue;

        *str = '\0';
        *arr++ = str+1;
    }

    return arr - src;
}
//---------------------------------------------------------------------------------------
static int get_num(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int str2arr(const char *str, void *out, int maxlen)
{
    int i, n = 0;

    while (*str)
    {
        while (*str && !isalnum((int)*str)) str++;

        uint8_t val = 0;
        for (i = 0; i < 2 && isalnum((int)*str); i++)
        {
            val <<= 4;
            val |= get_num(*str++);
        }
        if (i) ((uint8_t*)out)[n++] = val;
        if (n >= maxlen) break;
    }
    return n;
}

void reverse(void *buff, size_t len)
{
    uint8_t *rearp = (uint8_t *) buff + len - 1;
    uint8_t *headp = (uint8_t *) buff;

    while (headp < rearp)
    {
        swap(*headp, *rearp);
        headp++;
        rearp--;
    }
}

uint16_t calc_crc16(uint16_t crc, const void *buf, int size)
{
    uint8_t i;
    uint8_t *_buf = (uint8_t *) buf;

    while (size-- != 0)
    {
        for (i = 0x80; i != 0; i >>= 1)
        {
            if ((crc & 0x8000) != 0)
            {
                crc <<= 1;
                crc ^= 0x1021;
            }
            else
            {
                crc <<= 1;
            }
            if (((*_buf) & i) != 0)
            {
                crc ^= 0x1021;
            }
        }
        _buf++;
    }
    return crc;
}

//---------------------------------------------------------------------------------------
