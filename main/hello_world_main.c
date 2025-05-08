#include <stdio.h>
#include "esp_log.h"
#include "sounds.h"
#include "wav.h"
#include "launchpad_hal.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include <string.h>

#define TAG __FILE__

static const int16_t quiet[WAV_samplerate] = {0};

void app_main(void)
{
    Launchpad_handle_t hw_handle;
    ESP_ERROR_CHECK(launchpad_hal_init(&hw_handle));
    xTaskCreatePinnedToCore(launchpad_hal_input_task, "input_task", (1 << 12), &hw_handle, 10, NULL, 1);
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
            MAX98357A_close(&hw_handle.amp_handle, &amp_config);
            return;
        }
        WAV_set_volume(wavs + i, 0.02);
        // char filename[16];
        // snprintf(filename, sizeof(filename), "sound%d.wav", i);
        // ESP_LOGI(TAG, "Writing sound %d to file %s", i, filename);
        //// check if writing and reading from the sd card works
        // ESP_ERROR_CHECK(SDMMC_write_file(&hw_handle.sdmmc_handle, filename, wavs[i].data, wavs[i].data_sz));
        // ESP_LOGI(TAG, "Reading sound %d from file %s", i, filename);
        // void *data = NULL;
        // size_t size = 0;
        // ESP_ERROR_CHECK(SDMMC_read_file(&hw_handle.sdmmc_handle, filename, &data, &size));
        // if (data == NULL || size != wavs[i].data_sz)
        //{
        //     ESP_LOGE(TAG, "Failed to read sound %d from file %s", i, filename);
        //     for (size_t j = 0; j <= i; j++)
        //     {
        //         free(wavs[j].data);
        //     }
        //     MAX98357A_close(&hw_handle.amp_handle, &amp_config);
        //     return;
        // }
        // if (memcmp(data, wavs[i].data, wavs[i].data_sz) != 0)
        //{
        //     ESP_LOGE(TAG, "Data mismatch for sound %d", i);
        //     free(data);
        //     for (size_t j = 0; j <= i; j++)
        //     {
        //         free(wavs[j].data);
        //     }
        //     MAX98357A_close(&hw_handle.amp_handle, &amp_config);
        //     return;
        // }
        // free(data);
        // ESP_LOGI(TAG, "Sound %d written and read successfully", i);
    }
    while (true)
    {
        for (size_t i = 0; i < num_sounds; i++)
        {
            MAX98357A_play(&hw_handle.amp_handle, &amp_config, wavs[i].data, wavs[i].data_sz);
            MAX98357A_play(&hw_handle.amp_handle, &amp_config, quiet, sizeof(quiet));
        }
    }
}