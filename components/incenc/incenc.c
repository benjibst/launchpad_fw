#include <stdio.h>
#include "incenc.h"

esp_err_t incenc_init(const incenc_config_t *cfg, incenc_handle_t *handle)
{
    pcnt_unit_config_t pcnt_config = {
        .high_limit = 10,
        .low_limit = -10,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&pcnt_config, &handle->unit_handle));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 10000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(handle->unit_handle, &filter_config));

    pcnt_chan_config_t chan_a_config =
        {
            .edge_gpio_num = cfg->A,
            .level_gpio_num = cfg->B,
        };
    ESP_ERROR_CHECK(pcnt_new_channel(handle->unit_handle, &chan_a_config, &handle->chan_a_handle));
    pcnt_chan_config_t chan_b_config =
        {
            .edge_gpio_num = cfg->B,
            .level_gpio_num = cfg->A,
        };
    ESP_ERROR_CHECK(pcnt_new_channel(handle->unit_handle, &chan_b_config, &handle->chan_b_handle));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(handle->chan_a_handle, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(handle->chan_a_handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(handle->chan_b_handle, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(handle->chan_b_handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(handle->unit_handle));
    ESP_ERROR_CHECK(pcnt_unit_enable(handle->unit_handle));
    return ESP_OK;
}

esp_err_t incenc_get_cnt(incenc_handle_t *handle, int *cnt)
{
    ESP_ERROR_CHECK(pcnt_unit_get_count(handle->unit_handle, cnt));
    return ESP_OK;
}
esp_err_t incenc_start(incenc_handle_t *handle)
{
    ESP_ERROR_CHECK(pcnt_unit_start(handle->unit_handle));
    return ESP_OK;
}
esp_err_t incenc_stop(incenc_handle_t *handle)
{
    ESP_ERROR_CHECK(pcnt_unit_stop(handle->unit_handle));
    return ESP_OK;
}
esp_err_t incenc_reset_cnt(incenc_handle_t *handle)
{
    ESP_ERROR_CHECK(pcnt_unit_clear_count(handle->unit_handle));
    return ESP_OK;
}
