#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "MCP23017.h"

#define IO1_GPA_shift 0
#define IO2_GPA_shift 8
#define IO2_GPB_shift 16

typedef enum
{
    TRACK1_LED = (1 << (IO1_GPA_shift + 7)),
    TRACK2_LED = (1 << (IO2_GPB_shift + 5)),
    TRACK3_LED = (1 << (IO2_GPB_shift + 4)),

    PROFILE1_LED = (1 << (IO1_GPA_shift + 2)),
    PROFILE2_LED = (1 << (IO1_GPA_shift + 1)),
    PROFILE3_LED = (1 << (IO1_GPA_shift + 0)),

    EFFECT_LED = (1 << (IO2_GPB_shift + 7)),

    MASTER_MUTE_LED = (1 << (IO2_GPA_shift + 0)),

    TRACK1_MUTE_LED = (1 << (IO1_GPA_shift + 4)),
    TRACK2_MUTE_LED = (1 << (IO1_GPA_shift + 5)),
    TRACK3_MUTE_LED = (1 << (IO1_GPA_shift + 6)),

    REC_LED = (1 << (IO2_GPB_shift + 6)),
    REC_SEQ_LED = (1 << (IO1_GPA_shift + 3))
} LED_bits_t;

typedef uint32_t LED_state_t;

esp_err_t set_led(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led);
esp_err_t clear_led(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led);
esp_err_t toggle_led(MCP23017_handle_t *io1, MCP23017_handle_t *io2, LED_state_t *ledstate, LED_bits_t led);
