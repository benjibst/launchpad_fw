#include "wav.h"
#include "esp_log.h"
#include <stddef.h>
#include <string.h>
#include "esp_heap_caps.h"

#define TAG "WAV"

int WAV_check_req(const unsigned char *data, size_t size, size_t *data_sz_out, int16_t **wav_data_out)
{
    if (size < 44)
    {
        ESP_LOGE(TAG, "File too small");
        return -1;
    }
    // target values that are required for the wav file
    const char *riff_str = "RIFF";
    const uint32_t sz = size - 8;
    const char *wave_str = "WAVE";
    const char *fmt_str = "fmt ";
    const uint16_t audio_format = 1;
    const uint16_t num_channels = WAV_channels;
    const uint32_t sample_rate = WAV_samplerate;
    const uint32_t byte_rate = WAV_samplerate * WAV_channels * WAV_bytes_per_sample;
    const uint16_t block_align = WAV_channels * WAV_bytes_per_sample;
    const uint16_t bits_per_sample = WAV_bytes_per_sample << 3;
    const char *data_str = "data";
    if (memcmp(data + 0, riff_str, 4) != 0)
    {
        ESP_LOGE(TAG, "Not a RIFF file");
        return -1;
    }
    if (memcmp(data + 4, &sz, 4) != 0)
    {
        ESP_LOGE(TAG, "Size mismatch");
        return -1;
    }
    if (memcmp(data + 8, wave_str, 4) != 0)
    {
        ESP_LOGE(TAG, "Not a WAVE file");
        return -1;
    }
    if (memcmp(data + 12, fmt_str, 4) != 0)
    {
        ESP_LOGE(TAG, "'fmt ' missing");
        return -1;
    }

    // ffmpeg wav files include additional data in the fmt chunk, so compute where data starts
    uint32_t fmt_sz;
    memcpy(&fmt_sz, data + 16, 4);

    if (memcmp(data + 20, &audio_format, 2) != 0)
    {
        ESP_LOGE(TAG, "Audio format != 1");
        return -1;
    }
    if (memcmp(data + 22, &num_channels, 2) != 0)
    {
        ESP_LOGE(TAG, "Number of channels != 1");
        return -1;
    }
    if (memcmp(data + 24, &sample_rate, 4) != 0)
    {
        ESP_LOGE(TAG, "Sample rate mismatch");
        return -1;
    }
    if (memcmp(data + 28, &byte_rate, 4) != 0)
    {
        ESP_LOGE(TAG, "Byte rate mismatch");
        return -1;
    }
    if (memcmp(data + 32, &block_align, 2) != 0)
    {
        ESP_LOGE(TAG, "Block align mismatch");
        return -1;
    }
    if (memcmp(data + 34, &bits_per_sample, 2) != 0)
    {
        ESP_LOGE(TAG, "Bits per sample mismatch");
        return -1;
    }

    unsigned char *next_hdr = (unsigned char *)data + 20 + fmt_sz;
    while (next_hdr - data < size && memcmp(next_hdr, data_str, 4) != 0)
    {
        uint32_t chunk_size;
        memcpy(&chunk_size, next_hdr + 4, 4);
        next_hdr += 8 + chunk_size;
    }
    if (next_hdr - data >= size)
    {
        ESP_LOGE(TAG, "Data chunk not found");
        return -1;
    }
    unsigned char *data_loc = next_hdr;
    unsigned char *data_ptr = (unsigned char *)next_hdr + 8;
    size_t tot_hdr_sz = data_ptr - data;
    size_t data_sz = size - tot_hdr_sz;

    if (memcmp(data_loc + 4, &data_sz, 4) != 0)
    {
        ESP_LOGE(TAG, "Data size mismatch");
        return -1;
    }

    *data_sz_out = data_sz;
    *wav_data_out = (int16_t *)(data_ptr);
    return 0;
}

int WAV_read_bin(const unsigned char *data, size_t size, WAV_data_t *wav)
{
    size_t data_sz;
    int16_t *data_loc;
    if (WAV_check_req(data, size, &data_sz, &data_loc) != 0)
    {
        return -1;
    }
    wav->data = (int16_t *)heap_caps_malloc(data_sz, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (wav->data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return -1;
    }
    memcpy(wav->data, data_loc, data_sz);
    wav->data_sz = data_sz;
    wav->n_samples = data_sz / WAV_bytes_per_sample;
    return 0;
}
void WAV_set_volume(WAV_data_t *wav, float volume)
{
    for (int i = 0; i < wav->n_samples; i++)
    {
        wav->data[i] *= volume;
    }
}