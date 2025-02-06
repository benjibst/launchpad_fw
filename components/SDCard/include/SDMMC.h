#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
typedef struct
{
    gpio_num_t clk;
    gpio_num_t cmd;
    gpio_num_t d0;
    gpio_num_t d1;
    gpio_num_t d2;
    gpio_num_t d3;
    gpio_num_t det;
} SDMMC_config_t;

typedef struct
{
    sdmmc_card_t *card;
    const char *mountpoint;
} SDMMC_handle_t;

esp_err_t SDMMC_init(const SDMMC_config_t *cfg, SDMMC_handle_t *handle);
esp_err_t SDMMC_write_file(SDMMC_handle_t *handle, const char *filename, const void *data, size_t size);
