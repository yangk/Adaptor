#ifndef _ADAPTOR_H_
#define _ADAPTOR_H_

#include <stdint.h>
#include "sdk_evt.h"
#include "protocol.h"
#include "flash.h"

//#define SMART_MODEM

#define ADAPTOR_VERSION         0x0200
#define ADAPTOR_DATE            0x160713
#define ADAPTOR_VERSION_EXT     "SSC1663-ADPT-V30A010"

#define PLC_MATERIAL_CODE       "83676800003"
#define ADAPTOR_MATERIAL_CODE   "83676800004"

enum LAYER_TYPE
{
    LAYER_PLC, LAYER_ADAPTOR, LAYER_MCU, LAYER_NR
};

enum APP_TYPE
{
    APP_TYPE_NORMAL,
    APP_TYPE_LBS,
};

#pragma pack(1)
struct AdaptorSetting
{
    uint8_t aid[AID_LEN];
};
#pragma pack()

struct Adaptor
{
    uint8_t type;
    uint8_t aid[AID_LEN];
    uint8_t sid[SID_LEN];
    uint8_t gid[AID_LEN];
    uint8_t panid[SID_LEN];
    uint8_t mac[MAC_LEN];

    uint32_t baud, parity;

    uint8_t regtype : 1;
};
extern struct Adaptor adaptor;

struct LocalOps
{
    const char *name;
    int cmd;
    int (*handle)(uint8_t *data, int len);
};

extern const char *adaptor_ext_ver;
extern const char *adaptor_soft_type;
extern const char *adaptor_plc_type;

void adaptor_show(void);
void adaptor_init(void);

const char *get_dev_ver(void);
const char *get_plc_ver(void);
int get_dev_type_by_layer(enum LAYER_TYPE type, uint8_t *dev_type);
enum LAYER_TYPE get_layer_by_dev_type(const uint8_t *dev_type);
int get_dev_type(uint8_t *type);
int get_plc_type(uint8_t *type);
const char *get_soft_ver_by_layer(enum LAYER_TYPE type);

int do_set_psk(void);
int do_psk_resp(const uint8_t *dst, int state);
int do_plc_broadcast(int key_mod, const void *data, int len);
int do_plc_unicast(const uint8_t *dst, int key_mod, const void *data, int len);

int adaptor_on_uart1_received(const uint8_t *data, int len);

void adaptor_on_evt_devprop_req(const sdk_evt_devprop_req_t *req, const uint8_t *att, int len);
void adaptor_on_evt_rx_plc(const sdk_evt_rx_plc_t *info, const uint8_t *data, int len);
void adaptor_evt_sch_rpt(const sdk_evt_sch_rpt_t *resp);

#endif
