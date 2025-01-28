#pragma once

#include "driver/i2c_master.h"

extern const unsigned char CY8CMBR3116_config[128];

typedef struct
{
    gpio_num_t CHANGE_pin;
    uint8_t addr;
    uint32_t scl_speed;
} CY8CMBR3116_config_t;

typedef struct
{
    i2c_master_dev_handle_t i2c_slave;
} CY8CMBR3116_handle_t;

esp_err_t CY8CMBR3116_init(i2c_master_bus_handle_t bus, const CY8CMBR3116_config_t *cfg, CY8CMBR3116_handle_t *handle);
esp_err_t CY8CMBR3116_write(CY8CMBR3116_handle_t *handle, uint8_t *data, size_t len);
esp_err_t CY8CMBR3116_read(CY8CMBR3116_handle_t *handle, uint8_t *data, size_t len);
esp_err_t CY8CMBR3116_write_packet(CY8CMBR3116_handle_t *handle, uint8_t *data, size_t len);
esp_err_t CY8CMBR3116_read_packet(CY8CMBR3116_handle_t *handle, uint8_t *data, size_t len);
