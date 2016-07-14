#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>

//---------------------------------------------------------------------------------------
#define STC             0x7e
#define DID_LEN         0x02
#define SID_LEN         0x02
#define GID_LEN         0x02
#define AID_LEN         0x04
#define PANID_LEN		0x02
#define BUF_LEN			0xFF
#define EID_LEN			0x08
#define PW_LEN          0x02
#define PSK_LEN 		0x08

//---------------------------------------------------------------------------------------
#define CMD_SET_AID     0x01
#define CMD_GET_AID     0x03
#define CMD_ACK_AID     0x13
#define CMD_DEL_AID     0x04
#define CMD_REQ_AID     0x05
#define CMD_GET_SID     0x06
#define CMD_ACK_SID     0x16
#define CMD_GET_EID     0x07
#define CMD_ACK_EID     0x17
#define CMD_SET_BPS     0x08
#define CMD_SET_REG     0x09
#define CMD_UNLINK      0x0A
#define CMD_REGINFOR    0x0B
#define CMD_SET_PANID   0x0C
#define CMD_GET_GWAID   0x0D
#define CMD_GET_VER     0x0E
#define CMD_ACK_VER     0x1E
#define CMD_GET_PANID   0x0F
#define CMD_ACK_PANID   0x1F
#define CMD_TST_PLC     0x20
#define CMD_CHG_TONE    0x21

#define CMD_ACK         0x00
#define CMD_NAK         0xFF

#define CMD_SET         0x07
#define CMD_READ        0x02
#define CMD_UPDATE      0x06
#define CMD_RELI_REPORT     0x01
#define CMD_NRELI_REPORT    0x00

//---------------------------------------------------------------------------------------
#define NO_ERR        0x00
#define OTHER_ERR     0x0F
#define LEN_ERR       0x01
#define BUFFER_ERR    0x02
#define DATA_ERR      0x03
#define DID_ERR       0x04
#define DEV_BUSY      0x05
#define NO_RETURN     0x10
//---------------------------------------------------------------------------------------

#pragma pack(1)
struct SmartFrame
{
    uint8_t stc;
    uint8_t said[AID_LEN];
    uint8_t taid[AID_LEN];
    uint8_t seq;
    uint8_t len;
    uint8_t data[0];
};
#define SMART_FRAME_HEAD sizeof(struct SmartFrame)

struct AppFrame
{
    uint8_t cmd;
    uint8_t data[0];
};
struct GroupFrame
{
    uint8_t len : 6;
    uint8_t type: 2;
    uint8_t data[0];
};

struct Body
{
    uint8_t did[2];
    uint8_t ctrl;
    uint8_t data[0];
};
#define FBD_FRAME_HEAD  sizeof(struct Body)

struct RegData
{
    uint8_t aid[AID_LEN];
    uint8_t panid[SID_LEN];
    uint8_t pw[PW_LEN];
    uint8_t gid[AID_LEN];
    uint8_t sid[SID_LEN];
};
#pragma pack()

#define frame_len(frame) (SMART_FRAME_HEAD + frame->len + 1)

struct SmartFrame *get_smart_frame(const uint8_t *in, int len);
int is_my_smart_frame(const struct SmartFrame *frame);
int code_body(uint16_t did, int err, const void *data, int len, void *out, int maxlen);
int code_frame(const uint8_t *src, const uint8_t *dest, int seq, int cmd, 
    const uint8_t *data, int len, void *out, int maxlen);
int code_local_frame(const uint8_t *in, int len, void *out, int maxlen);
void do_evt_tst_resp(const sdk_evt_tst_resp_t *resp);

void aid_init(void);
int code_ret_frame(struct SmartFrame *pframe, int len);
int smart_frame_handle(struct SmartFrame *frame);
int do_plc_frame(const sdk_evt_rx_plc_t *info, struct SmartFrame *pframe);
int resend_check(const uint8_t *src, const struct SmartFrame *pframe);
#endif
