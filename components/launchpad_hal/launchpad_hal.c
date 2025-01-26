#include "launchpad_hal.h"
#include "esp_log.h"

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
};

const MCP23017_config_t io2_config = {
    .addr = 0b0100001,
    .scl_speed = 400000,
};
const gpio_num_t status_led = GPIO_NUM_15;

// io1: A:LED
// io1: B:button
esp_err_t launchpad_hal_setup_io1(Launchpad_handle_t *handle)
{
    ESP_LOGI("launchpad_hal", "Setting up IO1");
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IODIRA, 0x00));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPPUB, 0xFF));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPINTENB, 0xFF));
    return ESP_OK;
}

// io2: B:4-7:LED
// io2: A:0:LED
// io2: A:1-2:rotary encoder
// io2: A:3-7:button
esp_err_t launchpad_hal_setup_io2(Launchpad_handle_t *handle)
{
    ESP_LOGI("launchpad_hal", "Setting up IO2");
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRB, 0x0F));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPPUA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPINTENA, 0b11111110));
    return ESP_OK;
}

esp_err_t launchpad_hal_enable_all_led(Launchpad_handle_t *handle)
{
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPIOA, 0xFF));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPIOB, 0xF0));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPIOA, 0x01));
    ESP_ERROR_CHECK(gpio_set_level(status_led, 1));
    return ESP_OK;
}

esp_err_t launchpad_hal_disable_all_led(Launchpad_handle_t *handle)
{
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPIOA, 0x00));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPIOB, 0x00));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPIOA, 0x00));
    ESP_ERROR_CHECK(gpio_set_level(status_led, 0));
    return ESP_OK;
}

esp_err_t launchpad_hal_init(Launchpad_handle_t *handle)
{
    ESP_ERROR_CHECK(gpio_set_direction(status_led, GPIO_MODE_OUTPUT));

    ESP_ERROR_CHECK(MAX98357A_init(&amp_config, &handle->amp_handle));

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = i2c_bus_pins.SCL,
        .sda_io_num = i2c_bus_pins.SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };
    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus));

    ESP_ERROR_CHECK(i2c_master_probe(bus, io1_config.addr, 1000));
    ESP_ERROR_CHECK(MCP23017_init(bus, &io1_config, &handle->io1_handle));
    ESP_ERROR_CHECK(launchpad_hal_setup_io1(handle));

    ESP_ERROR_CHECK(i2c_master_probe(bus, io2_config.addr, 1000));
    ESP_ERROR_CHECK(MCP23017_init(bus, &io2_config, &handle->io2_handle));
    ESP_ERROR_CHECK(launchpad_hal_setup_io2(handle));
    return ESP_OK;
}
