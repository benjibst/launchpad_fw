#include "driver/i2c_master.h"
#include "driver/gpio.h"

typedef enum
{
    MCP23017_IODIRA = 0x00,
    MCP23017_IODIRB = 0x01,
    MCP23017_IPOLA = 0x02,
    MCP23017_IPOLB = 0x03,
    MCP23017_GPINTENA = 0x04,
    MCP23017_GPINTENB = 0x05,
    MCP23017_DEFVALA = 0x06,
    MCP23017_DEFVALB = 0x07,
    MCP23017_INTCONA = 0x08,
    MCP23017_INTCONB = 0x09,
    MCP23017_IOCON = 0x0A,
    // MCP23017_IOCON = 0x0B,
    MCP23017_GPPUA = 0x0C,
    MCP23017_GPPUB = 0x0D,
    MCP23017_INTFA = 0x0E,
    MCP23017_INTFB = 0x0F,
    MCP23017_INTCAPA = 0x10,
    MCP23017_INTCAPB = 0x11,
    MCP23017_GPIOA = 0x12,
    MCP23017_GPIOB = 0x13,
    MCP23017_OLATA = 0x14,
    MCP23017_OLATB = 0x15,
} MCP23017_regaddr_t;

typedef struct
{
    gpio_num_t INTA_pin;
    gpio_num_t INTB_pin;
    uint8_t addr;
    uint32_t scl_speed;
} MCP23017_config_t;

typedef struct
{
    i2c_master_dev_handle_t i2c_slave;
} MCP23017_handle_t;

esp_err_t MCP23017_init(i2c_master_bus_handle_t bus, const MCP23017_config_t *cfg, MCP23017_handle_t *handle);
esp_err_t MCP23017_write_reg(MCP23017_handle_t *handle, MCP23017_regaddr_t reg, uint8_t data);
esp_err_t MCP23017_read_reg(MCP23017_handle_t *handle, MCP23017_regaddr_t reg, uint8_t *data);
