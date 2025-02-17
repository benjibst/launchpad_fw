
#include "led_defs.h"

#define GET_IO1_GPA(x) ((uint8_t)((x >> IO1_GPA_shift) & 0xFF))
#define GET_IO2_GPA(x) ((uint8_t)((x >> IO2_GPA_shift) & 0xFF))
#define GET_IO2_GPB(x) ((uint8_t)((x >> IO2_GPB_shift) & 0xFF))

esp_err_t write_target_gpio(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led)
{
    if (led >= (1 << IO1_GPA_shift) && led < (1 << IO2_GPA_shift))
    {
        return MCP23017_write_reg(io1, MCP23017_GPIOA, GET_IO1_GPA(*ledstate));
    }
    if (led >= (1 << IO2_GPA_shift) && led < (1 << IO2_GPB_shift))
    {
        return MCP23017_write_reg(io2, MCP23017_GPIOA, GET_IO2_GPA(*ledstate));
    }
    if (led >= (1 << IO2_GPB_shift) && led < (1 << (IO2_GPB_shift + 8)))
    {
        return MCP23017_write_reg(io2, MCP23017_GPIOB, GET_IO2_GPB(*ledstate));
    }
    return ESP_FAIL;
}

esp_err_t set_led(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led)
{
    if (*ledstate & led)
    {
        return ESP_OK;
    }
    *ledstate |= led;
    return write_target_gpio(io1, io2, ledstate, led);
}
esp_err_t clear_led(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led)
{
    if (!(*ledstate & led))
    {
        return ESP_OK;
    }
    *ledstate &= ~led;
    return write_target_gpio(io1, io2, ledstate, led);
}
esp_err_t toggle_led(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led)
{
    *ledstate ^= led;
    return write_target_gpio(io1, io2, ledstate, led);
}
