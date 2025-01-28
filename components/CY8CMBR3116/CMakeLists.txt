idf_component_register(SRCS "CY8CMBR3xxx_CRC.c" "CY8CMBR3xxx_APIs.c" "CY8CMBR3xxx_HostFunctions.c" "CY8CMBR3116.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_i2c esp_driver_gpio esp_timer)
