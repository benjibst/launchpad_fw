#include "hw_config.h"

const I2C_bus_pins_t i2c_bus_pins = {.SDA = GPIO_NUM_39,
                                     .SCL = GPIO_NUM_21};
const MAX98357A_config_t amp_config = {
    .LRCLK_pin = GPIO_NUM_6,
    .BCLK_pin = GPIO_NUM_5,
    .DIN_pin = GPIO_NUM_4,
    .SD_PIN = GPIO_NUM_7};
const MCP23017_config_t io1_config = {
    .addr = 0b0100000,
    .scl_speed = 400000,
    .INTB_pin = GPIO_NUM_8,
};
const MCP23017_config_t io2_config = {
    .addr = 0b0100001,
    .scl_speed = 400000,
    .INTA_pin = GPIO_NUM_45,
};
const SDMMC_config_t sdmmc_config = {
    .clk = GPIO_NUM_43,
    .cmd = GPIO_NUM_44,
    .d0 = GPIO_NUM_2,
    .d1 = GPIO_NUM_1,
    .d2 = GPIO_NUM_41,
    .d3 = GPIO_NUM_42,
    .det = GPIO_NUM_40,
};
const gpio_num_t status_led = GPIO_NUM_15;
const incenc_config_t master_vol_enc_config = {
    .A = GPIO_NUM_14,
    .B = GPIO_NUM_13,
};
const incenc_config_t track_vol_enc_config = {
    .A = GPIO_NUM_12,
    .B = GPIO_NUM_11,
};
const incenc_config_t seq_vol_enc_config = {
    .A = GPIO_NUM_10,
    .B = GPIO_NUM_9,
};
const incenc_config_t bpm_enc_config = {
    .A = GPIO_NUM_46,
    .B = GPIO_NUM_3,
};
