idf_component_register(SRCS "max98357.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES fatfs driver sd_spi button)
