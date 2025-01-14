#include "wav.h"
#include "esp_log.h"
#include <stddef.h>
#include <string.h>
#include "esp_heap_caps.h"

#define TAG "WAV"

int WAV_check_req(const unsigned char *data, size_t size, WAV_data_info_t *info)
{
    if (size < 44)
    {
        ESP_LOGE(TAG, "File too small");
        return -1;
    }

    const char riff_str[] = "RIFF";
    if (memcmp(data + 0, riff_str, sizeof(riff_str) - 1) != 0)
    {
        ESP_LOGE(TAG, "Not a RIFF file");
        return -1;
    }
    const uint32_t sz = size - 8;
    if (memcmp(data + 4, &sz, 4) != 0)
    {
        ESP_LOGE(TAG, "Size mismatch");
        return -1;
    }
    const char wave_str[] = "WAVE";
    if (memcmp(data + 8, wave_str, sizeof(wave_str) - 1) != 0)
    {
        ESP_LOGE(TAG, "Not a WAVE file");
        return -1;
    }
    const char fmt_str[] = "fmt ";
    if (memcmp(data + 12, fmt_str, sizeof(fmt_str) - 1) != 0)
    {
        ESP_LOGE(TAG, "'fmt ' missing");
        return -1;
    }

    // ffmpeg wav files include additional data in the fmt chunk, so compute where data starts
    uint32_t fmt_sz;
    memcpy(&fmt_sz, data + 16, sizeof(fmt_sz));

    const uint16_t a_fmt_PCM = 1;
    if (memcmp(data + 20, &a_fmt_PCM, sizeof(a_fmt_PCM)) != 0)
    {
        ESP_LOGE(TAG, "Audio format != 1 (PCM)");
        return -1;
    }

    uint16_t channels;
    memcpy(&channels, data + 22, sizeof(channels));

    uint32_t sample_rate = WAV_samplerate;
    if (memcmp(data + 24, &sample_rate, sizeof(sample_rate)) != 0)
    {
        ESP_LOGE(TAG, "Sample rate != %d", WAV_samplerate);
        return -1;
    }

    uint32_t byte_rate = sample_rate * channels * WAV_bytespersample;
    if (memcmp(data + 28, &byte_rate, sizeof(byte_rate)) != 0)
    {
        ESP_LOGE(TAG, "Byte rate mismatch");
        return -1;
    }

    uint16_t block_align = channels * WAV_bytespersample;
    if (memcmp(data + 32, &block_align, sizeof(block_align)) != 0)
    {
        ESP_LOGE(TAG, "Block align mismatch");
        return -1;
    }

    uint16_t bits_per_sample = WAV_bitdepth;
    if (memcmp(data + 34, &bits_per_sample, 2) != 0)
    {
        ESP_LOGE(TAG, "Bits per sample mismatch");
        return -1;
    }

    const char data_str[] = "data";
    unsigned char *next_hdr_id_ptr = (unsigned char *)data + 20 + fmt_sz;
    while (next_hdr_id_ptr - data < size && memcmp(next_hdr_id_ptr, data_str, sizeof(data_str) - 1) != 0)
    {
        uint32_t chunk_size;
        memcpy(&chunk_size, next_hdr_id_ptr + 4, sizeof(chunk_size));
        next_hdr_id_ptr += 8 + chunk_size;
    }
    if (next_hdr_id_ptr - data >= size)
    {
        ESP_LOGE(TAG, "Data chunk not found");
        return -1;
    }
    unsigned char *data_ptr = (unsigned char *)next_hdr_id_ptr + 8;
    size_t data_sz = data + size - data_ptr;
    if (memcmp(next_hdr_id_ptr + 4, &data_sz, 4) != 0)
    {
        ESP_LOGE(TAG, "Data size mismatch");
        return -1;
    }

    info->data = data_ptr;
    info->size = data_sz;
    info->channels = channels;
    return 0;
}

int WAV_convert_to_mono(WAV_data_info_t *info, WAV_data_mono16_t *wav)
{
    wav->data_sz = info->size / info->channels;
    wav->n_samples = wav->data_sz / WAV_bytespersample;
    wav->data = heap_caps_malloc(wav->data_sz, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if (!wav->data)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for wav data");
        return -1;
    }

    if (info->channels == 1)
    {
        memcpy(wav->data, info->data, info->size);
        return 0;
    }

    size_t stride = info->channels * WAV_bytespersample;
    int16_t *buf = heap_caps_malloc(stride, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    if (!buf)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for wav conversion buffer");
        return -1;
    }
    void *sample_ptr = info->data;
    for (size_t i = 0; i < wav->n_samples; i++)
    {
        memcpy(buf, sample_ptr, stride);
        int32_t sum = 0;
        for (size_t j = 0; j < info->channels; j++)
        {
            sum += buf[j];
        }
        wav->data[i] = sum / info->channels;
        sample_ptr += stride;
    }
    free(buf);
    return 0;
}

int WAV_read_bin(const unsigned char *data, size_t size, WAV_data_mono16_t *wav)
{
    WAV_data_info_t info;
    if (WAV_check_req(data, size, &info) != 0)
    {
        return -1;
    }
    ESP_LOGI(TAG, "Channels: %u, Sample rate: %d, Bytes per sample: %d", info.channels, WAV_samplerate, WAV_bytespersample);
    return WAV_convert_to_mono(&info, wav);
}
void WAV_set_volume(WAV_data_mono16_t *wav, float volume)
{
    for (int i = 0; i < wav->n_samples; i++)
    {
        wav->data[i] *= volume;
    }
}