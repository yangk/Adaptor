/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:
Description:
Author:			eastsoft
Version:		v1.0
Date:
History:
*/
/************************************************************************************************/

#include "include.h"
#include "ip_addr.h"

const ip_addr_t ip_addr_any = { IPADDR_ANY };
const ip_addr_t ip_addr_broadcast = { IPADDR_BROADCAST };

#ifndef isprint
#define in_range(c, lo, up)  ((u8)c >= lo && (u8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
u8 ip4_addr_netmask_valid(u32 netmask)
{
	u32 mask;
	u32 nm_hostorder = htonl(netmask);

	/* first, check for the first zero */
	for (mask = 1UL << 31 ; mask != 0; mask >>= 1)
	{
		if ((nm_hostorder & mask) == 0)
		{
			break;
		}
	}
	/* then check that there is no one */
	for (; mask != 0; mask >>= 1)
	{
		if ((nm_hostorder & mask) != 0)
		{
			/* there is a one after the first zero -> invalid */
			return 0;
		}
	}
	/* no one after the first zero -> valid */
	return 1;
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
u32 ipaddr_addr(const char *cp)
{
	ip_addr_t val;

	if (ipaddr_aton(cp, &val))
	{
		return ip4_addr_get_u32(&val);
	}
	return (IPADDR_NONE);
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
int ipaddr_aton(const char *cp, ip_addr_t *addr)
{
	u32 val;
	u8 base;
	char c;
	u32 parts[4];
	u32 *pp = parts;

	c = *cp;
	for (;;)
	{
		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, 1-9=decimal.
		 */
		if (!isdigit(c))
		{
			return (0);
		}
		val = 0;
		base = 10;
		if (c == '0')
		{
			c = *++cp;
			if (c == 'x' || c == 'X')
			{
				base = 16;
				c = *++cp;
			}
			else
			{
				base = 8;
			}
		}
		for (;;)
		{
			if (isdigit(c))
			{
				val = (val * base) + (int)(c - '0');
				c = *++cp;
			}
			else if (base == 16 && isxdigit(c))
			{
				val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
				c = *++cp;
			}
			else
			{
				break;
			}
		}
		if (c == '.')
		{
		  /*
		   * Internet format:
		   *  a.b.c.d
		   *  a.b.c   (with c treated as 16 bits)
		   *  a.b (with b treated as 24 bits)
		   */
			if (pp >= parts + 3)
			{
				return (0);
			}
			*pp++ = val;
			c = *++cp;
		}
		else
		{
			break;
		}
	}
  /*
   * Check for trailing characters.
   */
	if (c != '\0' && !isspace(c))
	{
		return (0);
	}
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
	switch (pp - parts + 1)
	{
		case 0:
			return (0);										/* initial nondigit */

		case 1:												/* a -- 32 bits */
			break;

		case 2:												/* a.b -- 8.24 bits */
			if (val > 0xffffffUL)
			{
				return (0);
			}
			val |= parts[0] << 24;
			break;

		case 3:												/* a.b.c -- 8.8.16 bits */
			if (val > 0xffff)
			{
				return (0);
			}
			val |= (parts[0] << 24) | (parts[1] << 16);
			break;

		case 4:												/* a.b.c.d -- 8.8.8.8 bits */
			if (val > 0xff)
			{
				return (0);
			}
			val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
			break;
		default:
			break;
	}
	if (addr)
	{
		ip4_addr_set_u32(addr, htonl(val));
	}
	return (1);
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
char * ipaddr_ntoa(const ip_addr_t *addr)
{
	static char str[16];
	return ipaddr_ntoa_r(addr, str, 16);
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
char *ipaddr_ntoa_r(const ip_addr_t *addr, char *buf, int buflen)
{
	u32 s_addr;
	char inv[3];
	char *rp;
	u8 *ap;
	u8 rem;
	u8 n;
	u8 i;
	int len = 0;

	s_addr = ip4_addr_get_u32(addr);

	rp = buf;
	ap = (u8 *)&s_addr;
	for(n = 0; n < 4; n++)
	{
		i = 0;
		do
		{
			rem = *ap % (u8)10;
			*ap /= (u8)10;
			inv[i++] = '0' + rem;
		} while(*ap);
		while(i--)
		{
			if (len++ >= buflen)
			{
				return NULL;
			}
			*rp++ = inv[i];
		}
		if (len++ >= buflen)
		{
			return NULL;
		}
		*rp++ = '.';
		ap++;
	}
	*--rp = 0;
	return buf;
}
