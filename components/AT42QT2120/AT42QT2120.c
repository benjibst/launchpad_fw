#include "AT42QT2120.h"
#include "esp_log.h"
#include <string.h>

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

    // ESP_ERROR_CHECK(AT42QT2120_write_1byte(handle, AT42QT2120_RESET, 1));

    uint8_t reg = 0;
    ESP_ERROR_CHECK(AT42QT2120_read(handle, AT42QT2120_CHIP_ID, &reg, 1));
    ESP_LOGI(TAG, "AT42QT2120 chip id: 0x%02X", reg);

    ESP_ERROR_CHECK(AT42QT2120_read(handle, AT42QT2120_FIRMWARE_VERSION, &reg, 1));
    ESP_LOGI(TAG, "AT42QT2120 firmware version: 0x%02X", reg);

    return ESP_OK;
}

esp_err_t AT42QT2120_write_1byte(AT42QT2120_handle_t *handle, AT42QT2120_regaddr_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    ESP_ERROR_CHECK(i2c_master_transmit(handle->i2c_slave, buf, 2, 100));
    return ESP_OK;
}
esp_err_t AT42QT2120_write_2bytes(AT42QT2120_handle_t *handle, AT42QT2120_regaddr_t reg, uint16_t data)
{
    uint8_t buf[3] = {reg, data & 0xff, (data >> 8) & 0xff};
    ESP_ERROR_CHECK(i2c_master_transmit(handle->i2c_slave, buf, 3, 100));
    return ESP_OK;
}

esp_err_t AT42QT2120_read(AT42QT2120_handle_t *handle, AT42QT2120_regaddr_t reg, uint8_t *data, size_t sz)
{
    uint8_t addr = reg;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(handle->i2c_slave, &addr, 1, data, sz, 100));
    return ESP_OK;
}
