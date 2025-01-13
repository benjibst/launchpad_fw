#pragma once
#include <stddef.h>
#include <stdint.h>

// if the wav file doesnt pass these requirements it will not be read
#define WAV_samplerate 44100
#define WAV_channels 1
#define WAV_bytes_per_sample 2

typedef struct
{
    size_t n_samples;
    size_t data_sz;
    int16_t *data;
} WAV_data_t;

int WAV_read_bin(const unsigned char *data, size_t size, WAV_data_t *wav);
void WAV_set_volume(WAV_data_t *wav, float volume);
