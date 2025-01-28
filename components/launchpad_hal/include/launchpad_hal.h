#include "MCP23017.h"
#include "MAX98357A.h"
#include "CY8CMBR3116.h"

typedef struct
{
    gpio_num_t SDA;
    gpio_num_t SCL;
} I2C_bus_pins_t;

extern const I2C_bus_pins_t i2c_bus_pins;
extern const MAX98357A_config_t amp_config;
extern const MCP23017_config_t io1_config;
extern const MCP23017_config_t io2_config;
extern const gpio_num_t status_led;

typedef struct
{
    MAX98357A_handle_t amp_handle;
    MCP23017_handle_t io1_handle;
    MCP23017_handle_t io2_handle;
    CY8CMBR3116_handle_t touch_handle;
    i2c_master_bus_handle_t i2c_bus;
} Launchpad_handle_t;

esp_err_t launchpad_hal_init(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_disable_all_led(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_enable_all_led(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_setup_io2(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_setup_io1(Launchpad_handle_t *handle);
