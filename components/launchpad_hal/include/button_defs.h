#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t IO1_GPB;
    uint8_t IO2_GPA;
} ButtonState_t;

bool track_button(ButtonState_t x);
bool profile_button(ButtonState_t x);
bool effect1_button(ButtonState_t x);
bool effect2_button(ButtonState_t x);
bool effect3_button(ButtonState_t x);
bool effect4_button(ButtonState_t x);
bool effect5_button(ButtonState_t x);
bool master_mute_button(ButtonState_t x);
bool track_mute_button(ButtonState_t x);
bool repeat1_button(ButtonState_t x);
bool repeat2_button(ButtonState_t x);
bool repeat3_button(ButtonState_t x);
bool delete_last_button(ButtonState_t x);
bool delete_all_button(ButtonState_t x);
bool recording_button(ButtonState_t x);

void print_button_pressed(ButtonState_t curr, ButtonState_t prev);
