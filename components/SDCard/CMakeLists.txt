idf_component_register(SRCS "SDMMC.c"
                    INCLUDE_DIRS "include"
                    REQUIRES fatfs esp_driver_gpio esp_driver_sdmmc sdmmc)
