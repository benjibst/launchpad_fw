#include "MCP23017.h"
#include "MAX98357A.h"
#include "SDMMC.h"
#include "button_defs.h"

typedef struct
{
    gpio_num_t SDA;
    gpio_num_t SCL;
} I2C_bus_pins_t;

typedef struct
{
    uint8_t IO1_GPA;
    uint8_t IO2_GPB;
    uint8_t IO2_GPA;
    uint8_t STATUS;
} LedState_t;

extern const I2C_bus_pins_t i2c_bus_pins;
extern const MAX98357A_config_t amp_config;
extern const MCP23017_config_t io1_config;
extern const MCP23017_config_t io2_config;
extern const SDMMC_config_t sdmmc_config;
extern const gpio_num_t status_led;

typedef struct
{
    MAX98357A_handle_t amp_handle;
    MCP23017_handle_t io1_handle;
    MCP23017_handle_t io2_handle;
    i2c_master_bus_handle_t i2c_bus;
    SDMMC_handle_t sdmmc_handle;
    ButtonState_t button_state;
} Launchpad_handle_t;

esp_err_t launchpad_hal_init(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_setup_io2(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_setup_io1(Launchpad_handle_t *handle);
void launchpad_hal_input_task(void *args);
