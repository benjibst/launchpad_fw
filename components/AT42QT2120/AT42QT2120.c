#include "AT42QT2120.h"
#include "esp_log.h"

#define TAG __FILE__

esp_err_t AT42QT2120_init(i2c_master_bus_handle_t bus, const AT42QT2120_config_t *cfg, AT42QT2120_handle_t *handle)
{
    i2c_device_config_t config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AT42QT2120_I2C_ADDR,
        .scl_speed_hz = cfg->scl_speed,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &config, &handle->i2c_slave));
    ESP_LOGI(TAG, "AT42QT2120 added to i2c bus");

    uint8_t reg = 0;

    ESP_ERROR_CHECK(AT42QT2120_read_reg(handle, AT42QT2120_CHIP_ID, &reg));
    ESP_LOGI(TAG, "AT42QT2120 chip id: 0x%02X", reg);

    ESP_ERROR_CHECK(AT42QT2120_read_reg(handle, AT42QT2120_FIRMWARE_VERSION, &reg));
    ESP_LOGI(TAG, "AT42QT2120 firmware version: 0x%02X", reg);

    return ESP_OK;
}

esp_err_t AT42QT2120_write_reg(AT42QT2120_handle_t *handle, AT42QT2120_regaddr_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    ESP_ERROR_CHECK(i2c_master_transmit(handle->i2c_slave, buf, 2, 100));
    return ESP_OK;
}
esp_err_t AT42QT2120_read_reg(AT42QT2120_handle_t *handle, AT42QT2120_regaddr_t reg, uint8_t *data)
{
    uint8_t buf[1] = {reg};
    ESP_ERROR_CHECK(i2c_master_transmit_receive(handle->i2c_slave, buf, 1, data, 1, 100));
    return ESP_OK;
}
