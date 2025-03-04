#include "hw_include.h"

esp_err_t launchpad_hal_init(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_cycle_leds(Launchpad_handle_t *handle, uint32_t delay_ms, uint32_t cycles);
esp_err_t launchpad_hal_setup_io2(Launchpad_handle_t *handle);
esp_err_t launchpad_hal_setup_io1(Launchpad_handle_t *handle);
void launchpad_hal_input_task(void *args);
