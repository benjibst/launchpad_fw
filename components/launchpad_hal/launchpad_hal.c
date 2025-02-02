#include "launchpad_hal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define TAG __FILE__

#define IO1_INTB_PIN GPIO_NUM_8
#define IO2_INTA_PIN GPIO_NUM_45

const I2C_bus_pins_t i2c_bus_pins = {.SDA = GPIO_NUM_39,
                                     .SCL = GPIO_NUM_21};
const MAX98357A_config_t amp_config = {
    .LRCLK_pin = GPIO_NUM_6,
    .BCLK_pin = GPIO_NUM_5,
    .DIN_pin = GPIO_NUM_4,
    .SD_PIN = GPIO_NUM_7};
const MCP23017_config_t io1_config = {
    .addr = 0b0100000,
    .scl_speed = 100000,
    .INTB_pin = IO1_INTB_PIN,
};
const MCP23017_config_t io2_config = {
    .addr = 0b0100001,
    .scl_speed = 100000,
    .INTA_pin = IO2_INTA_PIN,
};
const gpio_num_t status_led = GPIO_NUM_15;
const CY8CMBR3116_config_t touch_config = {
    .addr = 0x37,
    .scl_speed = 100000,
    .CHANGE_pin = GPIO_NUM_38};

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void launchpad_hal_input_task(void *arg)
{
    gpio_num_t io_num;
    MCP23017_int_regs_t regs;
    Launchpad_handle_t *launchpad_handle = (Launchpad_handle_t *)arg;
    while (true)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            switch (io_num)
            {
            case IO1_INTB_PIN:
                MCP23017_read_int_regs(&launchpad_handle->io1_handle, &regs);
                if (!regs.intfb)
                    break;
                for (int i = 0; i < 8; i++)
                {
                    if (regs.intfb & (1 << i))
                    {
                        if (regs.intcapb & (1 << i))
                        {
                            ESP_LOGI(TAG, "IO1B Button %d released", i);
                        }
                        else
                        {
                            ESP_LOGI(TAG, "IO1B Button %d pressed", i);
                        }
                    }
                }
                break;
            case IO2_INTA_PIN:
                MCP23017_read_int_regs(&launchpad_handle->io2_handle, &regs);
                if (!regs.intfa)
                    break;
                for (int i = 0; i < 8; i++)
                {
                    if (regs.intfb & (1 << i))
                    {
                        if (regs.intcapb & (1 << i))
                        {
                            ESP_LOGI(TAG, "IO2A Button %d released", i);
                        }
                        else
                        {
                            ESP_LOGI(TAG, "IO2A Button %d pressed", i);
                        }
                    }
                }
                break;
            default:
                ESP_LOGI(TAG, "Unknown interrupt");
                break;
            }
        }
    }
}

// io1: A:LED
// io1: B:button
esp_err_t launchpad_hal_setup_io1(Launchpad_handle_t *handle)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << io1_config.INTB_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    //  Set all pins as input
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IODIRA, 0x00));
    // Enable pull-up resistors
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPPUB, 0xFF));
    // Enable interrupt on change
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPINTENB, 0xFF));
    // Configure interrupt pin as open drain
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IOCON, 0b00000100));

    return ESP_OK;
}

// io2: B:4-7:LED
// io2: A:0:LED
// io2: A:1-2:rotary encoder
// io2: A:3-7:button
esp_err_t launchpad_hal_setup_io2(Launchpad_handle_t *handle)
{
    // gpio_config_t cfg = {
    //     .pin_bit_mask = 1ULL << io2_config.INTA_pin,
    //     .mode = GPIO_MODE_INPUT,
    //     .pull_up_en = GPIO_PULLUP_ENABLE,
    //     .intr_type = GPIO_INTR_NEGEDGE,
    // };
    // ESP_ERROR_CHECK(gpio_config(&cfg));
    // ESP_ERROR_CHECK(gpio_isr_handler_add(io2_config.INTA_pin, gpio_isr_handler, (void *)io2_config.INTA_pin));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRB, 0x0F));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPPUA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPINTENA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IOCON, 0b00000100));
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

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_register(&gpio_isr_handler, ))

    ESP_ERROR_CHECK(i2c_master_probe(bus, io1_config.addr, 1000));
    ESP_ERROR_CHECK(MCP23017_init(bus, &io1_config, &handle->io1_handle));
    ESP_ERROR_CHECK(launchpad_hal_setup_io1(handle));

    ESP_ERROR_CHECK(i2c_master_probe(bus, io2_config.addr, 1000));
    ESP_ERROR_CHECK(MCP23017_init(bus, &io2_config, &handle->io2_handle));
    ESP_ERROR_CHECK(launchpad_hal_setup_io2(handle));

    gpio_evt_queue = xQueueCreate(10, sizeof(gpio_num_t));
    if (gpio_evt_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create gpio event queue");
        return ESP_FAIL;
    }
    int ret = xTaskCreatePinnedToCore(&launchpad_hal_input_task, "input_task", (1 << 10), handle, 10, NULL, 1);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create input task");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(CY8CMBR3116_init(bus, &touch_config, &handle->touch_handle));
    return ESP_OK;
}
