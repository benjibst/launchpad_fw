
#include "launchpad_hal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <string.h>

#define TAG __FILE__

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    gpio_num_t gpio_num = (gpio_num_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void launchpad_input_loop_poll(Launchpad_handle_t *hw_handle)
{
    Button_state_t curr_state;
    int master_vol_cnt, track_vol_cnt, seq_vol_cnt, bpm_cnt;
    int master_vol_cnt_prev = 0, track_vol_cnt_prev = 0, seq_vol_cnt_prev = 0, bpm_cnt_prev = 0;
    while (true)
    {
        incenc_get_cnt(&hw_handle->master_vol_enc_handle, &master_vol_cnt);
        incenc_get_cnt(&hw_handle->track_vol_enc_handle, &track_vol_cnt);
        incenc_get_cnt(&hw_handle->seq_vol_enc_handle, &seq_vol_cnt);
        incenc_get_cnt(&hw_handle->bpm_enc_handle, &bpm_cnt);

        if (master_vol_cnt != master_vol_cnt_prev)
        {
            ESP_LOGI(TAG, "Master volume encoder: %d", master_vol_cnt);
            master_vol_cnt_prev = master_vol_cnt;
        }
        if (track_vol_cnt != track_vol_cnt_prev)
        {
            ESP_LOGI(TAG, "Track volume encoder: %d", track_vol_cnt);
            track_vol_cnt_prev = track_vol_cnt;
        }
        if (seq_vol_cnt != seq_vol_cnt_prev)
        {
            ESP_LOGI(TAG, "Sequence volume encoder: %d", seq_vol_cnt);
            seq_vol_cnt_prev = seq_vol_cnt;
        }
        if (bpm_cnt != bpm_cnt_prev)
        {
            ESP_LOGI(TAG, "BPM encoder: %d", bpm_cnt);
            bpm_cnt_prev = bpm_cnt;
        }

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
}
void launchpad_input_loop_interrupt(Launchpad_handle_t *hw_handle)
{
    gpio_num_t io_num;
    MCP23017_int_regs_t regs;
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(io1_config.INTB_pin, gpio_isr_handler, (void *)io1_config.INTB_pin));
    ESP_ERROR_CHECK(gpio_isr_handler_add(io2_config.INTA_pin, gpio_isr_handler, (void *)io2_config.INTA_pin));
    while (true)
    {

        ESP_LOGI(TAG, "Waiting for interrupt");
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            if (io_num == io1_config.INTB_pin)
            {
                printf("IO1B interrupt\n");
                MCP23017_read_int_regs(&hw_handle->io1_handle, &regs);
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
            }
            else if (io_num == io2_config.INTA_pin)
            {
                printf("IO2A interrupt\n");
                MCP23017_read_int_regs(&hw_handle->io2_handle, &regs);
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
            }
            else
            {
                ESP_LOGI(TAG, "Unknown interrupt");
            }
        }
    }
}

#define POLL 1
void launchpad_hal_input_task(void *args)
{
    Launchpad_handle_t *hw_handle = (Launchpad_handle_t *)args;

#ifdef POLL
    launchpad_input_loop_poll(hw_handle);
#else
    launchpad_input_loop_interrupt(hw_handle);
#endif
}

// io1: A:LED
// io1: B:button
esp_err_t launchpad_hal_setup_io1(Launchpad_handle_t *handle)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1 << io1_config.INTB_pin,
        .mode = GPIO_MODE_INPUT,
        //.pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    //  Set all pins as input
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IODIRA, 0x00));
    // Inverted input logic state (buttons pull down)
    // ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IPOLB, 0xFF));
    // Enable pull-up resistors
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPPUB, 0xFF));
    // Enable interrupt on change
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_GPINTENB, 0xFF));
    // Configure interrupt pin as active high
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io1_handle, MCP23017_IOCON, 0b00000010));
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
        //.pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRB, 0x0F));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IODIRA, 0b11111110));
    // ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IPOLA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPPUA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_GPINTENA, 0b11111110));
    ESP_ERROR_CHECK(MCP23017_write_reg(&handle->io2_handle, MCP23017_IOCON, 0b00000010));
    return ESP_OK;
}

esp_err_t launchpad_hal_init(Launchpad_handle_t *handle)
{
    memset(handle, 0, sizeof(Launchpad_handle_t));

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

    // event queue for gpio interrupts
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

    ESP_ERROR_CHECK(launchpad_hal_cycle_leds(handle, 200, 1));

    ESP_ERROR_CHECK(incenc_init(&master_vol_enc_config, &handle->master_vol_enc_handle));
    ESP_ERROR_CHECK(incenc_start(&handle->master_vol_enc_handle));
    ESP_ERROR_CHECK(incenc_init(&track_vol_enc_config, &handle->track_vol_enc_handle));
    ESP_ERROR_CHECK(incenc_start(&handle->track_vol_enc_handle));
    ESP_ERROR_CHECK(incenc_init(&seq_vol_enc_config, &handle->seq_vol_enc_handle));
    ESP_ERROR_CHECK(incenc_start(&handle->seq_vol_enc_handle));
    ESP_ERROR_CHECK(incenc_init(&bpm_enc_config, &handle->bpm_enc_handle));
    ESP_ERROR_CHECK(incenc_start(&handle->bpm_enc_handle));

    return ESP_OK;
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
