idf_component_register(SRCS "ui_led.c" "lv_port.c" "001_lvgl_init.c" "images/ikun.c"
                    INCLUDE_DIRS "."
                    REQUIRES st7789_driver lvgl cst816t_driver)
