#include <stdio.h>
#include "SDMMC.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "sdmmc_cmd.h"

#define TAG __FILE__
static const char *mountpoint = "/fat";
esp_err_t SDMMC_init(const SDMMC_config_t *cfg, SDMMC_handle_t *handle)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "Initializing SD card");
    handle->mountpoint = mountpoint;
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
    ret = esp_vfs_fat_sdmmc_mount(mountpoint, &host, &slot_config, &mount_config, &handle->card);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount filesystem. Error: %s", esp_err_to_name(ret));
        return ret;
    }
    sdmmc_card_print_info(stdout, handle->card);
    printf("Status: %d\n", sdmmc_get_status(handle->card));
    return ESP_OK;
}

esp_err_t SDMMC_write_file(SDMMC_handle_t *handle, const char *filename, const void *data, size_t size)
{
    char filepath[64];
    snprintf(filepath, sizeof(filepath), "%s/%s", handle->mountpoint, filename);
    ESP_LOGI(TAG, "Writing file %s", filepath);
    FILE *f = fopen(filepath, "wb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing %s", esp_err_to_name(ESP_FAIL));
        return ESP_FAIL;
    }
    if (fwrite(data, 1, size, f) != size)
    {
        ESP_LOGE(TAG, "Failed to write file");
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}
esp_err_t SDMMC_read_file(SDMMC_handle_t *handle, const char *filename, void **data, size_t *size)
{
    char filepath[64];
    snprintf(filepath, sizeof(filepath), "%s/%s", handle->mountpoint, filename);
    ESP_LOGI(TAG, "Reading file %s", filepath);
    FILE *f = fopen(filepath, "rb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading %s", esp_err_to_name(ESP_FAIL));
        return ESP_FAIL;
    }
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *data = malloc(*size);
    if (*data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for file data");
        fclose(f);
        return ESP_FAIL;
    }
    if (fread(*data, 1, *size, f) != *size)
    {
        ESP_LOGE(TAG, "Failed to read file data");
        free(*data);
        fclose(f);
        return ESP_FAIL;
    }
    fclose(f);
    return ESP_OK;
}
