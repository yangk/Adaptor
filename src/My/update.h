#ifndef _UPDATE_H_
#define _UPDATE_H_

#include <stdint.h>
#include "protocol.h"

#define UPDATE_FINISHED 0xFFFF

#pragma pack(1)
struct McuUpdateInfo
{
    uint32_t image_size;
    uint16_t crc;
    uint8_t  block_size;
    uint8_t  update_approved;
    uint8_t  mcu_image_in_63;
};
#define UPDATE_FILE_INFO    offsetof(struct McuUpdateInfo, update_approved)
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
int do_update(uint8_t cmd, const uint8_t *in, int len);
int resp_update(const uint8_t *in, int len);
int mcu_update_hook(const struct SmartFrame *frame);
#endif
