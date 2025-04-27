#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "lv_port.h"
#include "lvgl.h"
#include "st7789_driver.h"
#include "ui_led.h"

// 主函数
void app_main(void)
{
    lv_port_init();                 //初始化LVGL
    ui_led_create();
    st7789_lcd_backlight(true);         //打开背光
    while(1)
    {
        lv_task_handler();          //LVGL循环处理
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}