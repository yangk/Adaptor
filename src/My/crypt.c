#include "crypt.h"

#define DOWN_UNIT_TM 64
#define UP_UNIT_TM 80
#define NODE_PROCESS_TIME 7
#define NODE_AVGDELAY_TIME 1000
#define NODE_TRAVERSAL_TIME(T) (NODE_PROCESS_TIME + NODE_AVGDELAY_TIME + (T))
#define ACK_WAIT_TIME (61 + NODE_PROCESS_TIME + 5)
#define NET_TRAVERSAL_TIMEA(T, D) ((D) * (NODE_TRAVERSAL_TIME(T) + ACK_WAIT_TIME))
#define NET_REPEAT_TIME(T, REP) ((REP) * (NODE_TRAVERSAL_TIME(T) + ACK_WAIT_TIME))

int encrypt(unsigned char *frm, int len) {
	unsigned short datalen;
	unsigned char csum = 0;
	unsigned char cxor = 0;
	unsigned char seed;
	if (frm[0] != 0x9F)
		return -1;
	datalen = (frm[2] << 8) | frm[1];

	if ((datalen + 5 ) > len)
		return -1;
	int i;
	for (i = 3; i < datalen + 3; i++) {
		csum += frm[i];
		cxor ^= frm[i];
	}
	if (csum != frm[datalen + 3])
		return -1;
	if (cxor != frm[datalen + 4])
		return -1;
	unsigned short c = (datalen ^ 0x2129) + 0x0A5A;
	frm[1] = c & 0xFF;
	frm[2] = (c >> 8) & 0xFF;
	seed = csum ^ cxor;
	frm[3] ^= seed;
	for (i = 4; i < datalen + 3; i++)
		frm[i] ^= frm[i - 1];
	return (datalen + 5);
}

int decrypt(unsigned char *frm, int len) {
	unsigned short datalen;
	unsigned char csum = 0, cxor = 0;
	unsigned char tmp[255];

	if (len > sizeof(tmp))
		return -1;
	int i;
	for (i = 0; i < len; i++)
		tmp[i] = frm[i];
	if (frm[0] != 0x9F)
		return -1;
	datalen = (frm[2] << 8) | frm[1];
	datalen = (datalen - 0x0A5A) ^ 0x2129;

	if (datalen + 5 > len)
		return -1;

	for (i = datalen + 2; i > 3; i--)
		/* decry */
		tmp[i] ^= tmp[i - 1];
	tmp[3] ^= tmp[datalen + 3] ^ tmp[datalen + 4];

	for (i = 3; i < datalen + 3; ++i) /* check csum and cxor */
	{
		csum += tmp[i];
		cxor ^= tmp[i];
	}
	if (csum != tmp[datalen + 3] || cxor != tmp[datalen + 4])
		return -1;
	for (i = 0; i < datalen + 5; i++)
		frm[i] = tmp[i];

	frm[2] = (datalen >> 8) & 0xFF;
	frm[1] = datalen & 0xFF;
	return (datalen + 5);
}

int timeout(int hops) {
	if (hops <= 0 || hops > 8)
		return -1;

	int rtimes = (hops + 1) >> 1;

	int downtm=  NET_TRAVERSAL_TIMEA(DOWN_UNIT_TM, hops) + NET_REPEAT_TIME(DOWN_UNIT_TM, rtimes);
	int uptm= NET_TRAVERSAL_TIMEA(UP_UNIT_TM, hops) + NET_REPEAT_TIME(UP_UNIT_TM, rtimes);

	return (downtm + uptm);
}
