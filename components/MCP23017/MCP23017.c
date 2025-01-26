#include "MCP23017.h"
#include "esp_log.h"

esp_err_t MCP23017_init(i2c_master_bus_handle_t bus, const MCP23017_config_t *cfg, MCP23017_handle_t *handle)
{
    ESP_LOGI("MCP23017", "Initializing MCP23017");
    i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = cfg->addr,
        .scl_speed_hz = cfg->scl_speed,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &config, &handle->i2c_slave));
    ESP_LOGI("MCP23017", "MCP23017 added to i2c bus");
    return ESP_OK;
}

esp_err_t MCP23017_write_reg(MCP23017_handle_t *handle, MCP23017_regaddr_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    ESP_ERROR_CHECK(i2c_master_transmit(handle->i2c_slave, buf, 2, 100));
    return ESP_OK;
}
esp_err_t MCP23017_read_reg(MCP23017_handle_t *handle, MCP23017_regaddr_t reg, uint8_t *data)
{
    uint8_t buf[1] = {reg};
    ESP_ERROR_CHECK(i2c_master_transmit_receive(handle->i2c_slave, buf, 1, data, 1, 100));
    return ESP_OK;
}
