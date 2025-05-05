#pragma once

#include "MCP23017.h"
#include "MAX98357A.h"
#include "AT42QT2120.h"
#include "SDMMC.h"
#include "led_defs.h"
#include "button_defs.h"
#include "incenc.h"

typedef struct
{
    gpio_num_t SDA;
    gpio_num_t SCL;
} I2C_bus_pins_t;

typedef struct
{
    MAX98357A_handle_t amp_handle;
    MCP23017_handle_t io1_handle;
    MCP23017_handle_t io2_handle;
    AT42QT2120_handle_t touch_handle;
    i2c_master_bus_handle_t i2c_bus;
    SDMMC_handle_t sdmmc_handle;
    Button_state_t button_state;
    LED_state_t led_state;
    incenc_handle_t master_vol_enc_handle;
    incenc_handle_t track_vol_enc_handle;
    incenc_handle_t seq_vol_enc_handle;
    incenc_handle_t bpm_enc_handle;
} Launchpad_handle_t;

extern const I2C_bus_pins_t i2c_bus_pins;
extern const MAX98357A_config_t amp_config;
extern const MCP23017_config_t io1_config;
extern const MCP23017_config_t io2_config;
extern const AT42QT2120_config_t touch_config;
extern const SDMMC_config_t sdmmc_config;
extern const gpio_num_t status_led;
extern const incenc_config_t master_vol_enc_config;
extern const incenc_config_t track_vol_enc_config;
extern const incenc_config_t seq_vol_enc_config;
extern const incenc_config_t bpm_enc_config;