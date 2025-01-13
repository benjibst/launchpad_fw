#include "driver/gpio.h"
#include "driver/i2s_std.h"

#define MAX98357A_sample_rate 44100
#define MAX98357A_data_width I2S_DATA_BIT_WIDTH_16BIT

typedef struct
{
    gpio_num_t LRCLK_pin, BCLK_pin, DIN_pin, SD_PIN;
} MAX98357A_config_t;

typedef struct
{
    i2s_chan_handle_t tx_handle;
    MAX98357A_config_t cfg;
} MAX98357A_handle_t;

esp_err_t MAX98357A_init(MAX98357A_config_t *config, MAX98357A_handle_t *handle);

void MAX98357A_close(MAX98357A_handle_t *handle);

esp_err_t MAX98357A_play(MAX98357A_handle_t *handle, const int16_t *data, size_t len);

void generate_sine_wave(int16_t *data, size_t len, float freq, uint32_t sample_rate, uint32_t *start);