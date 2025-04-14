#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdint.h>

QueueHandle_t queueMsg;
TickType_t timeout = 5000;

typedef struct data
{
    uint32_t ID;
    char msg[30];
} data;

void userTask(void *param)
{
    data d_A = {1, "Hello I'm A"};

    while (1)
    {
        if (xQueueSend(queueMsg, &d_A, timeout) == pdTRUE)
        {
            vTaskDelay(2000);
            ESP_LOGI("User", "sent successfully");
        }
        vTaskDelay(10);
    }
}

void LCDTask(void *param)
{
    data d_LCD;

    while (1)
    {
        if (xQueueReceive(queueMsg, &d_LCD, timeout) == pdTRUE)
        {
            vTaskDelay(3000);
            ESP_LOGI("LCD", "receive successfully, ID: %" PRIu32 " , msg: %s", d_LCD.ID, d_LCD.msg);
        }
        vTaskDelay(10);
    }
}

void app_main(void)
{
    queueMsg = xQueueCreate(8, sizeof(data));

    ESP_LOGI("TAG", "begin");
    xTaskCreatePinnedToCore(userTask, "UserA", 1024 * 3, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(LCDTask, "LCD", 1024 * 3, NULL, 3, NULL, 1);
}
