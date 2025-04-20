idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES sd_spi driver max98357 button led)
