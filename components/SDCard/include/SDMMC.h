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
} SDMMC_handle_t;

esp_err_t SDMMC_init(SDMMC_config_t *cfg, SDMMC_handle_t *handle, const char *mountpoint);
