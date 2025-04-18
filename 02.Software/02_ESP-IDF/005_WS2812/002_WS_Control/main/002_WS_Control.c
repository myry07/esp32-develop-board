#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ws2812.h"
#include "button.h"
#include <inttypes.h>

#define WS_PIN 48
#define WS_NUM 1
#define BUTTON_PIN 0

#define TAG "WS&BUTTON"

ws2812_strip_handle_t wsHandle = NULL;
static TaskHandle_t ws_task_handle = NULL;

void short_press_cb(void)
{
    ESP_LOGI(TAG, "short press\n");
    xTaskNotify(ws_task_handle, 1, eSetValueWithOverwrite);
}

void long_press_cb(void)
{
    ESP_LOGI(TAG, "long press\n");
    xTaskNotify(ws_task_handle, 2, eSetValueWithOverwrite);
}

void ws2812Task(void *param)
{
    uint32_t notification_value;

    int index = 0;
    uint32_t r = 30;
    uint32_t g = 0;
    uint32_t b = 30;

    uint32_t choose = 1;

    while (1)
    {
        if (xTaskNotifyWait(0, 0, &notification_value, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            if (notification_value == 1)
            {
                if (choose == 1)
                {
                    if (r > 255)
                        r = 255;
                    r += 5;
                }
                else if (choose == 2)
                {
                    if (g > 255)
                        g = 255;
                    g += 5;
                }
                else if (choose == 3)
                {
                    if (b > 255)
                        b = 255;
                    b += 5;
                }
            }
            else if (notification_value == 2)
            {
                if (choose == 4)
                    choose = 1;
                choose++;
            }
            ESP_LOGI(TAG, "R: %" PRIu32 ", G: %" PRIu32 ", B: %" PRIu32, r, g, b);
        }

        esp_err_t err = ws2812_write(wsHandle, index, r, g, b);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    ws2812_init(WS_PIN, WS_NUM, &wsHandle);

    button_config_t cfg = {
        .gpio_num = BUTTON_PIN,
        .active_level = 0,       // GPIO0 接地为按下
        .long_press_time = 1500, // 1500ms 判定为长按
        .short_cb = short_press_cb,
        .long_cb = long_press_cb,
    };

    if (button_event_set(&cfg) == ESP_OK)
    {
        ESP_LOGI(TAG, "button init");
    }

    xTaskCreate(ws2812Task, "ws", 1024 * 4, NULL, 3, &ws_task_handle);
}