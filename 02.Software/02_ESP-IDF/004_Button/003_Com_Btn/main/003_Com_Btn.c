#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "button.h"

void short_press_cb(void)
{
    printf("short press\n");
}

void long_press_cb(void)
{
    printf("long press\n");
}

void app_main(void)
{
    button_config_t cfg = {
        .gpio_num = 0,
        .active_level = 0,       // GPIO0 接地为按下
        .long_press_time = 1500, // 1500ms 判定为长按
        .short_cb = short_press_cb,
        .long_cb = long_press_cb,
    };

    button_event_set(&cfg);
}