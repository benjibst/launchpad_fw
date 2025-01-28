#include <stdio.h>
#include "esp_log.h"
#include "sounds.h"
#include "wav.h"
#include "launchpad_hal.h"

static const int16_t quiet[WAV_samplerate] = {0};

void app_main(void)
{
    Launchpad_handle_t hw_handle;
    ESP_ERROR_CHECK(launchpad_hal_init(&hw_handle));
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
        WAV_set_volume(wavs + i, 0.1);
    }
    while (true)
    {
        for (size_t i = 0; i < num_sounds; i++)
        {
            launchpad_hal_enable_all_led(&hw_handle);
            MAX98357A_play(&hw_handle.amp_handle, &amp_config, wavs[i].data, wavs[i].data_sz);
            launchpad_hal_disable_all_led(&hw_handle);
            MAX98357A_play(&hw_handle.amp_handle, &amp_config, quiet, sizeof(quiet));
        }
    }
}