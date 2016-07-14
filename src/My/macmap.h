#ifndef _MACMAP_H_
#define _MACMAP_H_

#include <stdint.h>

enum MAC_ATTR
{
    MAC_ATTR_TMLESS = 1 << 0,
};

int get_mac_by_id(const uint8_t *id, uint8_t *mac);
int get_sid_by_id(const uint8_t *id, uint8_t *sid);
int get_sid_by_mac(const uint8_t *mac, uint8_t *sid);
int get_id_by_mac(const uint8_t *mac, uint8_t *id);

int add_mac_by_id(const uint8_t *id, const uint8_t *mac, const uint8_t *sid, uint32_t attr);
void set_id_attr(const uint8_t *id, uint32_t attr);
void del_mac_by_id(const uint8_t *id);
void clear_map();
#endif
