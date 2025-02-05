#include <stdio.h>
#include "SDMMC.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "sdmmc_cmd.h"

#define TAG __FILE__
esp_err_t SDMMC_init(SDMMC_config_t *cfg, SDMMC_handle_t *handle, const char *mountpoint)
{
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 128,
        .allocation_unit_size = 16 * 1024,
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.flags &= ~SDMMC_HOST_FLAG_DDR;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 4;
    slot_config.gpio_cd = cfg->det;
    slot_config.clk = cfg->clk;
    slot_config.cmd = cfg->cmd;
    slot_config.d0 = cfg->d0;
    slot_config.d1 = cfg->d1;
    slot_config.d2 = cfg->d2;
    slot_config.d3 = cfg->d3;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(mountpoint, &host, &slot_config, &mount_config, &handle->card);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    sdmmc_card_print_info(stdout, handle->card);
    return ESP_OK;
}
