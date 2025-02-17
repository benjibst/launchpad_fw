#include "button_defs.h"
#include <stdio.h>

inline bool track_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x80; }
inline bool profile_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x01; }

inline bool effect1_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x40; }
inline bool effect2_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x08; }
inline bool effect3_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x02; }
inline bool effect4_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x04; }
inline bool effect5_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x20; }

inline bool master_mute_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x04; }
inline bool track_mute_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x02; }

inline bool repeat1_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x20; }
inline bool repeat2_button_pressed(Button_state_t x) { return x.IO1_GPB & 0x10; }
inline bool repeat3_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x40; }

inline bool delete_last_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x80; }
inline bool delete_all_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x10; }

inline bool recording_button_pressed(Button_state_t x) { return x.IO2_GPA & 0x08; }

typedef bool (*ButtonFunction)(Button_state_t);
#define BUTTON_FUNCTION_ENTRY(name) {name, #name}
typedef struct
{
    ButtonFunction function;
    const char *name;
} ButtonFunctionEntry;

static ButtonFunctionEntry button_functions[] = {
    BUTTON_FUNCTION_ENTRY(track_button_pressed),
    BUTTON_FUNCTION_ENTRY(profile_button_pressed),
    BUTTON_FUNCTION_ENTRY(effect1_button_pressed),
    BUTTON_FUNCTION_ENTRY(effect2_button_pressed),
    BUTTON_FUNCTION_ENTRY(effect3_button_pressed),
    BUTTON_FUNCTION_ENTRY(effect4_button_pressed),
    BUTTON_FUNCTION_ENTRY(effect5_button_pressed),
    BUTTON_FUNCTION_ENTRY(master_mute_button_pressed),
    BUTTON_FUNCTION_ENTRY(track_mute_button_pressed),
    BUTTON_FUNCTION_ENTRY(repeat1_button_pressed),
    BUTTON_FUNCTION_ENTRY(repeat2_button_pressed),
    BUTTON_FUNCTION_ENTRY(repeat3_button_pressed),
    BUTTON_FUNCTION_ENTRY(delete_last_button_pressed),
    BUTTON_FUNCTION_ENTRY(delete_all_button_pressed),
    BUTTON_FUNCTION_ENTRY(recording_button_pressed)};

void print_button_pressed(Button_state_t curr, Button_state_t prev)
{
    for (size_t i = 0; i < sizeof(button_functions) / sizeof(button_functions[0]); i++)
    {
        if (button_functions[i].function(curr) && !button_functions[i].function(prev))
        {
            printf("%s pressed\n", button_functions[i].name);
        }
    }
}
