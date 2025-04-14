#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

QueueHandle_t queueMsg;
TickType_t timeout = 200;

typedef struct data
{
    int ID;
    char msg[30];
    int num;
} data;

void userTask(void *param)
{
    data d1 = *(data *)param;

    while (1)
    {
        vTaskDelay(500);
        if (xQueueSend(queueMsg, &d1, timeout) == pdTRUE)
        {
            ESP_LOGI("User: ", "sent successfully num: %d", d1.num);
            d1.num++;
        }
        else
        {
            ESP_LOGI("User: ", "sent unsuccessfully");
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
            ESP_LOGI("LCD: ", "receive successfully, ID: %d , msg: %s, num: %d",
                     d_LCD.ID, d_LCD.msg, d_LCD.num);
        }
        else
        {
            ESP_LOGI("LCD: ", "receive unsuccessfully");
        }
        vTaskDelay(10);
    }
}

void app_main(void)
{
    queueMsg = xQueueCreate(8, sizeof(data));

    data d1 = {1, "Hello I'm Task1", 0};
    data d2 = {2, "Hello I'm Task2", 0};
    data d3 = {3, "Hello I'm Task3", 0};

    ESP_LOGI("Main: ", "begin");

    xTaskCreatePinnedToCore(userTask, "User1", 1024 * 3, (void *)&d1, 3, NULL, 1);
    xTaskCreatePinnedToCore(userTask, "User2", 1024 * 3, (void *)&d2, 3, NULL, 1);
    xTaskCreatePinnedToCore(userTask, "User3", 1024 * 3, (void *)&d3, 3, NULL, 1);
    xTaskCreatePinnedToCore(LCDTask, "LCD", 1024 * 3, NULL, 3, NULL, 1);
}
