
#include "launchpad_hal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <string.h>

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
    .scl_speed = 400000,
    .INTB_pin = IO1_INTB_PIN,
};
const MCP23017_config_t io2_config = {
    .addr = 0b0100001,
    .scl_speed = 400000,
    .INTA_pin = IO2_INTA_PIN,
};
const SDMMC_config_t sdmmc_config = {
    .clk = GPIO_NUM_43,
    .cmd = GPIO_NUM_44,
    .d0 = GPIO_NUM_2,
    .d1 = GPIO_NUM_1,
    .d2 = GPIO_NUM_41,
    .d3 = GPIO_NUM_42,
    .det = GPIO_NUM_40,
};
const gpio_num_t status_led = GPIO_NUM_15;

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    gpio_num_t gpio_num = (gpio_num_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void print_bin(uint8_t val)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%d", (val >> i) & 1);
    }
    printf("\n");
}
void launchpad_hal_input_task(void *args)
{
    gpio_num_t io_num;
    MCP23017_int_regs_t regs;
    Launchpad_handle_t *hw_handle = (Launchpad_handle_t *)args;
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(io1_config.INTB_pin, gpio_isr_handler, (void *)IO1_INTB_PIN));
    ESP_ERROR_CHECK(gpio_isr_handler_add(io2_config.INTA_pin, gpio_isr_handler, (void *)IO2_INTA_PIN));
    Button_state_t curr_state;
    while (true)
    {
        MCP23017_read_reg(&hw_handle->io1_handle, MCP23017_GPIOB, &curr_state.IO1_GPB);
        MCP23017_read_reg(&hw_handle->io2_handle, MCP23017_GPIOA, &curr_state.IO2_GPA);
        if (curr_state.IO1_GPB != hw_handle->button_state.IO1_GPB)
        {
            print_button_pressed(curr_state, hw_handle->button_state);
            hw_handle->button_state.IO1_GPB = curr_state.IO1_GPB;
        }
        if (curr_state.IO2_GPA != hw_handle->button_state.IO2_GPA)
        {
            print_button_pressed(curr_state, hw_handle->button_state);
            hw_handle->button_state.IO2_GPA = curr_state.IO2_GPA;
        }
    }
    while (true)
    {
        ESP_LOGI(TAG, "Waiting for interrupt");
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            switch (io_num)
            {
            case IO1_INTB_PIN:
                printf("IO1B interrupt\n");
                MCP23017_read_int_regs(&hw_handle->io1_handle, &regs);
                // print_int_regs_bin(&regs);
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
                printf("IO2A interrupt\n");
                MCP23017_read_int_regs(&hw_handle->io2_handle, &regs);
                // print_int_regs_bin(&regs);
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
        .pin_bit_mask = 1 << io1_config.INTB_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    //  Set all pins as input
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IODIRA, 0x00));
    // Inverted input logic state (buttons pull down)
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IPOLB, 0xFF));
    // Enable pull-up resistors
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPPUB, 0xFF));
    // Enable interrupt on change
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPINTENB, 0xFF));
    // Configure interrupt pin as active high
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IOCON, 0b00000100));
    return ESP_OK;
}

// io2: B:4-7:LED
// io2: A:0:LED
// io2: A:1-2:rotary encoder
// io2: A:3-7:button
esp_err_t launchpad_hal_setup_io2(Launchpad_handle_t *handle)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1 << io2_config.INTA_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRB, 0x0F));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IPOLA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPPUA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPINTENA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IOCON, 0b00000100));
    return ESP_OK;
}

esp_err_t launchpad_hal_init(Launchpad_handle_t *handle)
{
    memset(handle, 0, sizeof(Launchpad_handle_t));
    handle->button_state.IO1_GPB = 0;
    handle->button_state.IO2_GPA = 0;

    ESP_ERROR_CHECK(gpio_set_direction(status_led, GPIO_MODE_OUTPUT));
    gpio_set_level(status_led, 1);

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

    gpio_evt_queue = xQueueCreate(10, sizeof(gpio_num_t));
    if (gpio_evt_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create gpio event queue");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(i2c_master_probe(bus, io1_config.addr, 1000));
    ESP_ERROR_CHECK(MCP23017_init(bus, &io1_config, &handle->io1_handle));
    ESP_ERROR_CHECK(launchpad_hal_setup_io1(handle));

    ESP_ERROR_CHECK(i2c_master_probe(bus, io2_config.addr, 1000));
    ESP_ERROR_CHECK(MCP23017_init(bus, &io2_config, &handle->io2_handle));
    ESP_ERROR_CHECK(launchpad_hal_setup_io2(handle));

    ESP_ERROR_CHECK(SDMMC_init(&sdmmc_config, &handle->sdmmc_handle));

    launchpad_hal_cycle_leds(handle, 100, 3);
    return ESP_OK;
}

void print_binary_memory(const void *mem, size_t size, int width)
{
    const unsigned char *bytes = (const unsigned char *)mem;

    for (size_t i = 0; i < size; i++)
    {
        // Print byte in binary format
        for (int j = 7; j >= 0; j--)
        {
            printf("%d", (bytes[i] >> j) & 1);
        }
        printf(" ");

        if ((i + 1) % width == 0 || i + 1 == size)
        {
            printf("\n"); // New line after width bytes or at the end
        }
    }
}

esp_err_t set_and_clear_led(Launchpad_handle_t *hw_handle, uint32_t led, uint32_t delay_ms)
{
    ESP_ERROR_CHECK(set_led(&hw_handle->io1_handle, &hw_handle->io2_handle, &hw_handle->led_state, led));
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(clear_led(&hw_handle->io1_handle, &hw_handle->io2_handle, &hw_handle->led_state, led));
    return ESP_OK;
}
esp_err_t launchpad_hal_cycle_leds(Launchpad_handle_t *handle, uint32_t delay_ms, uint32_t cycles)
{
    while (cycles-- > 0)
    {
        ESP_ERROR_CHECK(set_and_clear_led(handle, TRACK1_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, TRACK2_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, TRACK3_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, PROFILE1_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, PROFILE2_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, PROFILE3_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, EFFECT_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, MASTER_MUTE_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, TRACK1_MUTE_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, TRACK2_MUTE_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, TRACK3_MUTE_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, REC_LED, delay_ms));
        ESP_ERROR_CHECK(set_and_clear_led(handle, REC_SEQ_LED, delay_ms));
    }
    return ESP_OK;
}
