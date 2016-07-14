#include "include.h"
#include "comfunc.h"
#include "adaptor.h"
#include "sec_printf.h"
#include "sdk_svc.h"
#include "cmd.h"

//---------------------------------------------------------------------------------------
static param_t cmd_adaptor_param_tab[] = {
	{PARAM_ARG|PARAM_TOGGLE, "", "show|reset|get|set|backup" },
    {PARAM_OPT|PARAM_INTEGER|PARAM_SUB|PARAM_OPTIONAL, "prlvl", "", "get"},
    {PARAM_OPT|PARAM_INTEGER|PARAM_SUB|PARAM_OPTIONAL, "prlvl", "", "set"},
    {PARAM_OPT|PARAM_INTEGER|PARAM_SUB|PARAM_OPTIONAL, "baddr", "", "set"},
	PARAM_EMPTY
};
static cmd_desc_t cmd_adaptor_desc = CMD_DESC_ENTRY(
							"", cmd_adaptor_param_tab
							);

static void docmd_adaptor(command_t *cmd, xsh_t *xsh)
{
    if (!strcmp(xsh->argv[1], "show"))
    {
        adaptor_show();
    } 
    else if (!strcmp(xsh->argv[1], "reset"))
    {
        const char *psk = "eastsoft";

        sdk_svc_psk_t info;

        strcpy((char*)info.psk, psk); 
        info.len = strlen(psk);

        sdk_svc_set_psk(&info);

        flash_image_bin_erase(USER_ADDR, USER_LEN);
    }
    else if (!strcmp(xsh->argv[1], "get")) 
    {
        if (strstr(xsh->argv[2], "prlvl")) 
        {
            esprintf("Sec print level: %d\n", sec_get_pr_lvl());
        }
    }
    else if (!strcmp(xsh->argv[1], "set")) 
    {
        if (strstr(xsh->argv[2], "prlvl")) 
        {
            sec_set_pr_lvl(strtoul(strchr(xsh->argv[2], '=') + 1, NULL, 10));
        }
        else if (strstr(xsh->argv[2], "baddr")) 
        {
            uint32_t addr = strtoul(strchr(xsh->argv[2], '=') + 1, NULL, 16);
            if (addr != IMAGEX_ADDR) 
            {
                esprintf("ImageX addr should be [0x%8X]\n", IMAGEX_ADDR);
                return;
            }

            flash_user_info_write(IMAGES_CFG, (uint8_t *)&addr, sizeof(addr));
        }
    }
    else if (!strcmp(xsh->argv[1], "backup")) 
    {
        uint32_t size;

        memcpy(&size, (void*)IMAGES_ADDR, sizeof(size));
        size += 8;
        if (size > IMAGES_LEN) 
        {
            esprintf("Image size should < [0x%8X]\n", IMAGES_LEN);
            return;
        }
        memcpy(sdk_update_image, (void*)IMAGES_ADDR, size);
        flash_image_bin_erase(IMAGEX_ADDR, size);
        flash_image_bin_write(IMAGEX_ADDR, sdk_update_image, size);
    }
}
//---------------------------------------------------------------------------------------
static view_t view_adaptor[] =
{
	VIEWTAB_ENTRY(adaptor,   "[adaptor] "),
};

static command_t cmd_adaptor[] =
{
	CMDTAB_ENTRY(adaptor, root,  adaptor,  docmd_adaptor, &cmd_adaptor_desc),
};

void sec_cmd_init(void)
{
    cmd_init(view_adaptor, array_size(view_adaptor), cmd_adaptor, array_size(cmd_adaptor)); 
}
//---------------------------------------------------------------------------------------
