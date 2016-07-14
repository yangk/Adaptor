#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "include.h"
#include "flash.h"
#include "sdk.h"
#include "comfunc.h"
#include "adaptor.h"
#include "update.h"
#include "timer.h"
#include "sdk_svc.h"
#include "protocol.h"
#include "uart.h"
#include "crc32.h"

//-------------------------------------------------------------------------------------------------
#define UPDATE_FINISHED 0xFFFF
#define ADAPTOR_IMAGE_INIT_CRC 0xFFFFFFFF
//-------------------------------------------------------------------------------------------------
struct UpdateCache
{
    uint32_t file_size; 
    uint32_t max_blk_cnt;
    uint32_t block_no;
    uint32_t block_size;
    uint16_t file_crc; 
    uint32_t file_type; 
    int waiting_for_reset;
    uint8_t ver[0x40];
};
static struct UpdateCache update_cache;
static estimer_t flash_update_tmr;
static uint32_t update_len = 0;
struct McuUpdateInfo mcu_update_info;
estimer_t mcu_reset_tmr;
//-------------------------------------------------------------------------------------------------
static bool check_adaptor_image(const uint8_t *image, uint32_t size)
{
    uint32_t len, crc, _crc, idx = 0;

    len = get_le_val(&image[idx], sizeof(len));
    idx += sizeof(len);
    crc = get_le_val(&image[idx], sizeof(crc));
    idx += sizeof(crc);

    _crc = get_crc32_s(ADAPTOR_IMAGE_INIT_CRC, &image[idx], size-idx);
    pr_info("%s: image crc[%08x], calc crc[%08x]\n", __func__, crc, _crc);

    return crc == _crc;
}
static void check_adaptor_config(void)
{
    uint32_t addr;

    memcpy((uint8_t *)&addr, (void *)IMAGES_CFG, sizeof(addr));
    if (addr == IMAGEX_ADDR) return;

    pr_emerg("%s: imageX addr[%08x] error! rewrite to be[%08x]\n", __func__, addr, IMAGEX_ADDR);
    addr = IMAGEX_ADDR;
    flash_user_info_write(IMAGES_CFG, (uint8_t *)&addr, sizeof(addr));
}
// if mcu not received?
static void req_mcu_reset(estimer_t tmr, void *arg)
{
    static uint8_t try_cnt;
    uint8_t tmp[0x20];
    uint8_t data[1] = {CMD_REQ_MCU_RST};

    int ret = code_local_frame(data, sizeof(data), tmp, sizeof(tmp));
    esuart_send_data(UART_CHN_1, tmp, ret);
    if(try_cnt == 0)    timer_start(mcu_reset_tmr);
    if(try_cnt++ > 3)
    {
        timer_stop(mcu_reset_tmr, TMR_PERIODIC);
        try_cnt = 0;
    }
}
static void flash_update_tmr_handle(estimer_t tmr, void *arg)
{
    sdk_err_t err = SDK_ERR_DEF;

    switch (update_cache.file_type) 
    {
    case LAYER_PLC:
        err = sdk_svc_update(update_len);
        break;
    case LAYER_ADAPTOR:
        if (check_adaptor_image(sdk_update_image, update_len)) 
        {
            check_adaptor_config();
            flash_image_bin_erase(IMAGES_ADDR, update_len);
            flash_image_bin_write(IMAGES_ADDR, sdk_update_image, update_len);
            err = SDK_ERR_OK;
        }
        break;
    case LAYER_MCU:
        flash_image_bin_erase(IMAGES_MCU_ADDR, update_len+UPDATE_FILE_INFO);
        memmove(sdk_update_image + UPDATE_FILE_INFO, sdk_update_image, update_len);
        memcpy(sdk_update_image, (uint8_t*)&mcu_update_info.image_size, UPDATE_FILE_INFO);
        flash_image_bin_write(IMAGES_MCU_ADDR, sdk_update_image, update_len+UPDATE_FILE_INFO);
        //flash_image_bin_write(IMAGES_MCU_ADDR, (uint8_t*)&mcu_update_info.image_size, UPDATE_FILE_INFO);
        //flash_image_bin_write(IMAGES_MCU_ADDR+UPDATE_FILE_INFO, sdk_update_image, update_len);        
        req_mcu_reset(NULL, NULL);
        break;
    }

    if (err == SDK_ERR_OK) 
    {
        delay10ms(10);
        sdk_svc_reset();
    }
    update_cache.waiting_for_reset = 0;
}
//-------------------------------------------------------------------------------------------------
static void catche_init(const struct UpdateFile *file, int len, struct UpdateCache *pcache)
{
    pcache->file_size = get_le_val(file->size, sizeof(file->size));
    pcache->block_size = file->block_size;
    pcache->max_blk_cnt = (pcache->file_size + pcache->block_size - 1) / pcache->block_size;
    pcache->file_crc = get_le_val(file->crc, sizeof(file->crc));
    pcache->block_no = 1;
    pcache->waiting_for_reset = 0;
    memcpy(pcache->ver, file->data, len);
}
//-------------------------------------------------------------------------------------------------
static int update_hframe_opt(struct Update *update, int len)
{
    uint8_t dev_type[8];
    const char *soft_ver;
    struct UpdateCache *pcache = &update_cache;
    enum LAYER_TYPE layer_type = LAYER_ADAPTOR;

    int seq_no = pcache->block_no;
    struct UpdateFile *file = (struct UpdateFile *)update->data;

    if (len <= UPDATE_FILE_HEAD)
    {
err_lbl:
        get_dev_type(dev_type);
        memcpy(file->type, dev_type, sizeof(dev_type));
        soft_ver = get_dev_ver();
        memcpy(file->data, soft_ver, strlen(soft_ver));
        put_le_val(0, update->seq, sizeof(update->seq));
        return UPDATE_FILE_HEAD + strlen(soft_ver);
    }

    layer_type = get_layer_by_dev_type(file->type);
    if (layer_type == LAYER_NR) goto err_lbl;

    len -= UPDATE_FILE_HEAD;
    soft_ver = get_soft_ver_by_layer(layer_type);
    if (!memcmp(file->data, soft_ver, strlen(soft_ver)))
    {
        seq_no = UPDATE_FINISHED;
        goto end_lbl;
    }

    if (pcache->file_size == get_le_val(file->size, sizeof(file->size))
        && pcache->file_crc == get_le_val(file->crc, sizeof(file->crc))
        && pcache->block_size == file->block_size
        && !memcmp(pcache->ver, file->data, len))
    {
        goto end_lbl;
    }
    catche_init(file, len, pcache);
    pcache->file_type = layer_type;
    seq_no = 1;

 end_lbl:
    put_le_val(seq_no, update->seq, sizeof(update->seq));
    return 0;
}
//-------------------------------------------------------------------------------------------------
static int update_mcu_hframe_opt(struct Update *update, int len)
{
    struct UpdateCache *pcache = &update_cache;

    // do not respond mcu update frame without permission
    if (mcu_update_info.mcu_image_in_63 != 1) return -1;

    if (len <= UPDATE_FILE_HEAD) return -1;

    len -= UPDATE_FILE_HEAD;
    struct UpdateFile *file = (struct UpdateFile *)update->data;
    if (pcache->file_size == get_le_val(file->size, sizeof(file->size))
        && pcache->file_crc == get_le_val(file->crc, sizeof(file->crc))
        && pcache->block_size == file->block_size)
    {
        goto end_lbl;
    }
    catche_init(file, len, pcache);
    pcache->file_type = LAYER_MCU;
    mcu_update_info.image_size = pcache->file_size;
    mcu_update_info.crc = pcache->file_crc;
    mcu_update_info.block_size = pcache->block_size;
    return -1;

 end_lbl:
    put_le_val(pcache->block_no, update->seq, sizeof(update->seq));
    return 0;
}
//-------------------------------------------------------------------------------------------------
static int check_file_crc(void)
{
    struct UpdateCache *pcache = &update_cache;

    return pcache->file_crc == calc_crc16(0, sdk_update_image, pcache->file_size);
}

static int update_dframe_opt(struct Update *pupdate, int len, int seq)
{
    struct UpdateCache *pcache = &update_cache;

    if (pcache->block_no != seq || len > pcache->block_size)
        goto end;

    int addr = (pcache->block_no - 1) * pcache->block_size;
    memcpy(sdk_update_image+addr, pupdate->data, len);

    if (++pcache->block_no > pcache->max_blk_cnt)
    {
    	update_len = pcache->file_size;
        if (!check_file_crc())
        {
            pcache->block_no = 1;
            goto end;
        }
        pcache->waiting_for_reset = 1;
        timer_start(flash_update_tmr);
        pcache->block_no = UPDATE_FINISHED;
    }

end:
    put_le_val(pcache->block_no, pupdate->seq, sizeof(pupdate->seq));
    return 0;
}
//-------------------------------------------------------------------------------------------------
int do_update(uint8_t cmd, const uint8_t *in, int len)
{
    int ret = 0;

    /* no respond for the update frame while waiting for the reset */
    if (update_cache.waiting_for_reset) return 0;

    if (!mcu_update_info.mcu_image_in_63 && CMD_UPDATE_MCU == cmd) return -1;

    struct Update *pupdate = (struct Update *)in;

    int seq = get_le_val(pupdate->seq, sizeof(pupdate->seq));
    if (seq == 0)
    {
        int (*update_header_hook)(struct Update *, int) 
            = cmd == CMD_UPDATE_MCU ? update_mcu_hframe_opt : update_hframe_opt;
        ret = update_header_hook(pupdate, pupdate->len); 
    }
    else
    {
        ret = update_dframe_opt(pupdate, pupdate->len, seq);
    }
    if (ret < 0) return ret;
    pupdate->len = ret;
    ret = pupdate->ack ? offsetof(struct Update, data) + pupdate->len : 0;
    pupdate->ack = 0;
    return ret;
}
int resp_update(const uint8_t *in, int len)
{

    return 0;
}
int mcu_update_hook(const struct SmartFrame *frame)
{
    struct AppFrame *app = (struct AppFrame *)frame->data;

    if (app->cmd == CMD_UPDATE_MCU && frame->len == UPDATE_FILE_INFO)
    {
        struct Update *update = (struct Update *)app->data;
        uint16_t crc = get_le_val(update->crc, sizeof(update->crc));
        uint16_t seq = get_le_val(update->seq, sizeof(update->seq));
        if (mcu_update_info.mcu_image_in_63
            && crc == mcu_update_info.crc
            && seq == 1)
        {
            mcu_update_info.update_approved = 1;
        }
    }
    return 0;            
}
void update_init(void)
{
    memset((uint8_t*)&mcu_update_info, 0x00, sizeof(mcu_update_info));
    memset(&update_cache, 0, sizeof(update_cache));

    flash_update_tmr = timer_create(200, 0, TMR_TRIGGER, flash_update_tmr_handle, NULL, "flash_update_tmr");
    if (!flash_update_tmr)
    {
        esprintf("creat flash_update_tmr timer failed!\n");
    }
    mcu_reset_tmr = timer_create(400, 400, TMR_PERIODIC, req_mcu_reset, NULL, "mcu_reset_tmr");
    if (!mcu_reset_tmr)
    {
        esprintf("creat mcu_reset_tmr timer failed!\n");
    }
}
//-------------------------------------------------------------------------------------------------
