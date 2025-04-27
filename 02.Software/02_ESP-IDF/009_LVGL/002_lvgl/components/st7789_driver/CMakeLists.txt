idf_component_register(SRCS "st7789_driver.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_common driver
                    PRIV_REQUIRES esp_timer esp_lcd)
