#pragma once

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

typedef struct
{
    gpio_num_t A, B;
} incenc_config_t;

typedef struct incenc_handle_t
{
    pcnt_unit_handle_t unit_handle;
    pcnt_channel_handle_t chan_a_handle;
    pcnt_channel_handle_t chan_b_handle;

} incenc_handle_t;

esp_err_t incenc_init(const incenc_config_t *cfg, incenc_handle_t *handle);
esp_err_t incenc_get_cnt(incenc_handle_t *handle, int *cnt);
esp_err_t incenc_start(incenc_handle_t *handle);
esp_err_t incenc_stop(incenc_handle_t *handle);
esp_err_t incenc_reset_cnt(incenc_handle_t *handle);