#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ws2812.h"

#define WS_PIN 48
#define WS_NUM 1

ws2812_strip_handle_t wsHandle = NULL;

void ws2812Task(void *param)
{
    int index = 0;
    uint32_t r = 30;
    uint32_t g = 0;
    uint32_t b = 30;

    while (1)
    {
        esp_err_t err = ws2812_write(wsHandle, index, r, g, b);
        if (err != ESP_OK)
        {
            ESP_LOGE("WS2812", "Write failed: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    ws2812_init(WS_PIN, WS_NUM, &wsHandle);
    xTaskCreate(ws2812Task, "ws", 1024 * 4, NULL, 3, NULL);
}
