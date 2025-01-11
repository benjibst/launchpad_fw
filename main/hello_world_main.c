#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "MAX98357A.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
const int64_t BUFLEN = 1 << 10;
void app_main(void)
{
    printf("Hello world!\n");
    MAX98357A_config_t config = {
        .LRCLK_pin = GPIO_NUM_4,
        .BCLK_pin = GPIO_NUM_5,
        .DIN_pin = GPIO_NUM_6,
        .SD_PIN = GPIO_NUM_7};
    MAX98357A_handle_t handle;
    MAX98357A_init(&config, &handle);
    ESP_LOGI("MAX98357A", "MAX98357A initialized");
    ESP_LOGI("MAX98357A", "Allocating");
    size_t bufsz = BUFLEN * sizeof(int16_t);
    int16_t *data = malloc(bufsz);
    if (!data)
    {
        ESP_LOGE("MAX98357A", "Failed to allocate memory");
        return;
    }
    uint32_t start = 0;
    while (true)
    {
        generate_sine_wave(data, BUFLEN, 500, MAX98357A_sample_rate, &start);
        MAX98357A_play(&handle, data, bufsz);
    }
}