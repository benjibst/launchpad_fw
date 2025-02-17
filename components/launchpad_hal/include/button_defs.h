#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t IO1_GPB;
    uint8_t IO2_GPA;
} Button_state_t;
bool track_button_pressed(Button_state_t x);
bool profile_button_pressed(Button_state_t x);
bool effect1_button_pressed(Button_state_t x);
bool effect2_button_pressed(Button_state_t x);
bool effect3_button_pressed(Button_state_t x);
bool effect4_button_pressed(Button_state_t x);
bool effect5_button_pressed(Button_state_t x);
bool master_mute_button_pressed(Button_state_t x);
bool track_mute_button_pressed(Button_state_t x);
bool repeat1_button_pressed(Button_state_t x);
bool repeat2_button_pressed(Button_state_t x);
bool repeat3_button_pressed(Button_state_t x);
bool delete_last_button_pressed(Button_state_t x);
bool delete_all_button_pressed(Button_state_t x);
bool recording_button_pressed(Button_state_t x);

void print_button_pressed(Button_state_t curr, Button_state_t prev);
