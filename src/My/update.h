#ifndef _UPDATE_H_
#define _UPDATE_H_

#include <stdint.h>

#pragma pack(1)
struct Update
{
    uint8_t seq[2];
    uint8_t ack;
    uint8_t crc[2];
    uint8_t len;
    uint8_t data[0];
};
#define UPDATE_HEAD sizeof(struct Update)
struct UpdateFile
{
    uint8_t size[4];
    uint8_t crc[2];
    uint8_t block_size;
    uint8_t type[8];
    uint8_t data[0];
};
#define UPDATE_FILE_HEAD sizeof(struct UpdateFile)
#pragma pack()
void update_init(void);
int do_update(const uint8_t *in, int len);
int resp_update(const uint8_t *in, int len);
#endif
