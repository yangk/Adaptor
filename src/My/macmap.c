#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "comfunc.h"
#include "macmap.h"

//---------------------------------------------------------------------------------------
#define MACMAP_NR   1000
//---------------------------------------------------------------------------------------
#pragma pack(1)
struct MacMap
{
    uint8_t aid[4];
    uint8_t mac[6];
    uint8_t sid[2];
    uint32_t seq;
};
#pragma pack()
static struct MacMap macmap[MACMAP_NR];
//---------------------------------------------------------------------------------------
static struct MacMap *get_macmap(const uint8_t *id, const uint8_t *mac)
{
    int i;

    for (i = 0; i < array_size(macmap); ++i)
    {
        if (id && !memcmp(id, macmap[i].aid, sizeof(macmap[i].aid)))
        {
            return &macmap[i]; 
        }
        if (mac && !memcmp(mac, macmap[i].mac, sizeof(macmap[i].mac)))
        {
            return &macmap[i]; 
        }
    }
    return NULL;
}
static struct MacMap *get_free_pos(void)
{
	int i;
	
    for (i = 0; i < array_size(macmap); ++i)
    {
        if (is_all_xx(macmap[i].aid, 0, sizeof(macmap[i].aid)))
        {
            return &macmap[i];
        }
    }

    struct MacMap *tmp = &macmap[0];

    for (i = 0; i < array_size(macmap); ++i)
    {
        if (time_after(tmp->seq, macmap[i].seq)) 
        {
            tmp = &macmap[i];
        }
    }
    return tmp; 
}
//---------------------------------------------------------------------------------------
int get_mac_by_id(const uint8_t *id, uint8_t *mac)
{
    struct MacMap *tmp = get_macmap(id, NULL);
    if (!tmp) return -1;

    memcpy(mac, tmp->mac, sizeof(tmp->mac));
    return 0; 
}
//---------------------------------------------------------------------------------------
int get_sid_by_id(const uint8_t *id, uint8_t *sid)
{
    struct MacMap *tmp = get_macmap(id, NULL);
    if (!tmp) return -1;

    memcpy(sid, tmp->sid, sizeof(tmp->sid));
    return 0; 
}
//---------------------------------------------------------------------------------------
int get_sid_by_mac(const uint8_t *mac, uint8_t *sid)
{
    struct MacMap *tmp = get_macmap(NULL, mac);
    if (!tmp) return -1;

    memcpy(sid, tmp->sid, sizeof(tmp->sid));
    return 0; 
}
//---------------------------------------------------------------------------------------
int get_id_by_mac(const uint8_t *mac, uint8_t *id)
{
    struct MacMap *tmp = get_macmap(NULL, mac);
    if (!tmp) return -1;

    memcpy(id, tmp->aid, sizeof(tmp->aid));
    return 0; 
}
//---------------------------------------------------------------------------------------
#define PERMANENT_SEQ 0xFFFFFFFF
static uint32_t get_seq(void)
{
    static uint32_t seq = 0;
    if (++seq == PERMANENT_SEQ) seq = 0;
    return seq;
}
int add_mac_by_id(const uint8_t *id, const uint8_t *mac, const uint8_t *sid, uint32_t attr)
{
    struct MacMap *pos = get_macmap(id, NULL);

    if (!pos) pos = get_free_pos(); 
    memcpy(pos->aid, id, sizeof(pos->aid));
    memcpy(pos->mac, mac, sizeof(pos->mac));
    if (sid) memcpy(pos->sid, sid, sizeof(pos->sid));
    if (pos->seq != PERMANENT_SEQ)
    {
        pos->seq = (attr & MAC_ATTR_TMLESS) ? PERMANENT_SEQ : get_seq();
    }

    return 0;
}
//---------------------------------------------------------------------------------------
int update_mac_by_id(const uint8_t *id, const uint8_t *mac, const uint8_t *sid, uint32_t attr)
{
    struct MacMap *tmp = get_macmap(id, NULL);
    if (!tmp) return -1;

    memcpy(tmp->mac, mac, sizeof(tmp->mac));
    memcpy(tmp->sid, sid, sizeof(tmp->sid));
    return 0; 
}
//---------------------------------------------------------------------------------------
void set_id_attr(const uint8_t *id, uint32_t attr)
{
    struct MacMap *tmp = get_macmap(id, NULL);
    if (!tmp) return;

    if (attr & MAC_ATTR_TMLESS)
        tmp->seq = PERMANENT_SEQ;
}
void del_mac_by_id(const uint8_t *id)
{
    struct MacMap *tmp = get_macmap(id, NULL);
    if (!tmp) return;

    memset(tmp, 0, sizeof(*tmp));
}
//---------------------------------------------------------------------------------------
void clear_map()
{
	memset(macmap, 0, sizeof(macmap));
}
//---------------------------------------------------------------------------------------
