#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "MAX98357A.h"
#include "esp_log.h"
#include "sounds.h"
#include "wav.h"

static const int16_t quiet[WAV_samplerate / 4] = {0};
#define LED_PIN GPIO_NUM_15
void app_main(void)
{
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    printf("Hello world!\n");
    while (true)
    {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Hello world!\n");
    MAX98357A_config_t config = {
        .LRCLK_pin = GPIO_NUM_4,
        .BCLK_pin = GPIO_NUM_5,
        .DIN_pin = GPIO_NUM_6,
        .SD_PIN = GPIO_NUM_7};
    MAX98357A_handle_t handle;
    MAX98357A_init(&config, &handle);
    WAV_data_mono16_t wavs[num_sounds];
    for (size_t i = 0; i < num_sounds; i++)
    {
        ESP_LOGI("WAV", "Reading sound %d", i);
        if (WAV_read_bin(sounds[i], *(sound_lengths[i]), &(wavs[i])))
        {
            for (size_t j = 0; j < i; j++)
            {
                // free all the previously read files
                free(wavs[j].data);
            }
            MAX98357A_close(&handle);
            return;
        }
        WAV_set_volume(wavs + i, 0.1);
    }
    while (true)
    {
        for (size_t i = 0; i < num_sounds; i++)
        {
            MAX98357A_play(&handle, wavs[i].data, wavs[i].data_sz);
            MAX98357A_play(&handle, quiet, sizeof(quiet));
        }
    }
}