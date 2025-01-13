idf_component_register(SRCS "hello_world_main.c"
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS ""
                    REQUIRES MAX98357A esp_timer sounds wav)
