idf_component_register(SRCS "leds.c" "button_defs.c" "launchpad_hal.c" "hw_config.c"
                    INCLUDE_DIRS "include"
                    REQUIRES MAX98357A MCP23017 SDCard incenc)
