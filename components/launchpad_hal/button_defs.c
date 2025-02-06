#include "button_defs.h"
#include <stdio.h>

inline bool track_button(ButtonState_t x) { return x.IO1_GPB & 0x80; }
inline bool profile_button(ButtonState_t x) { return x.IO1_GPB & 0x01; }

inline bool effect1_button(ButtonState_t x) { return x.IO1_GPB & 0x40; }
inline bool effect2_button(ButtonState_t x) { return x.IO1_GPB & 0x08; }
inline bool effect3_button(ButtonState_t x) { return x.IO1_GPB & 0x02; }
inline bool effect4_button(ButtonState_t x) { return x.IO1_GPB & 0x04; }
inline bool effect5_button(ButtonState_t x) { return x.IO1_GPB & 0x20; }

inline bool master_mute_button(ButtonState_t x) { return x.IO2_GPA & 0x04; }
inline bool track_mute_button(ButtonState_t x) { return x.IO2_GPA & 0x02; }

inline bool repeat1_button(ButtonState_t x) { return x.IO2_GPA & 0x20; }
inline bool repeat2_button(ButtonState_t x) { return x.IO1_GPB & 0x10; }
inline bool repeat3_button(ButtonState_t x) { return x.IO2_GPA & 0x40; }

inline bool delete_last_button(ButtonState_t x) { return x.IO2_GPA & 0x80; }
inline bool delete_all_button(ButtonState_t x) { return x.IO2_GPA & 0x10; }

inline bool recording_button(ButtonState_t x) { return x.IO2_GPA & 0x08; }

typedef bool (*ButtonFunction)(ButtonState_t);
#define BUTTON_FUNCTION_ENTRY(name) {name, #name}
typedef struct
{
    ButtonFunction function;
    const char *name;
} ButtonFunctionEntry;

static ButtonFunctionEntry button_functions[] = {
    BUTTON_FUNCTION_ENTRY(track_button),
    BUTTON_FUNCTION_ENTRY(profile_button),
    BUTTON_FUNCTION_ENTRY(effect1_button),
    BUTTON_FUNCTION_ENTRY(effect2_button),
    BUTTON_FUNCTION_ENTRY(effect3_button),
    BUTTON_FUNCTION_ENTRY(effect4_button),
    BUTTON_FUNCTION_ENTRY(effect5_button),
    BUTTON_FUNCTION_ENTRY(master_mute_button),
    BUTTON_FUNCTION_ENTRY(track_mute_button),
    BUTTON_FUNCTION_ENTRY(repeat1_button),
    BUTTON_FUNCTION_ENTRY(repeat2_button),
    BUTTON_FUNCTION_ENTRY(repeat3_button),
    BUTTON_FUNCTION_ENTRY(delete_last_button),
    BUTTON_FUNCTION_ENTRY(delete_all_button),
    BUTTON_FUNCTION_ENTRY(recording_button)};

void print_button_pressed(ButtonState_t curr, ButtonState_t prev)
{
    for (size_t i = 0; i < sizeof(button_functions) / sizeof(button_functions[0]); i++)
    {
        if (button_functions[i].function(curr) && !button_functions[i].function(prev))
        {
            printf("%s pressed\n", button_functions[i].name);
        }
    }
}
