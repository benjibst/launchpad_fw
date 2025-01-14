#pragma once
#include <stddef.h>
#include <stdint.h>

#define WAV_samplerate 44100
#define WAV_bitdepth 16
#define WAV_bytespersample (WAV_bitdepth / 8)

typedef struct
{
    size_t n_samples;
    size_t data_sz;
    int16_t *data;
} WAV_data_mono16_t;

typedef struct
{
    void *data;
    size_t size;
    uint16_t channels;
} WAV_data_info_t;

int WAV_read_bin(const unsigned char *data, size_t size, WAV_data_mono16_t *wav);
void WAV_set_volume(WAV_data_mono16_t *wav, float volume);
