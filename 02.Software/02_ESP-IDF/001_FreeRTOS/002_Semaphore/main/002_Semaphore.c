#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "SEMA"

SemaphoreHandle_t sema = NULL;

void sendTask(void *param)
{
    while (1)
    {
        if (xSemaphoreGive(sema) == pdTRUE)
        {
            ESP_LOGI(TAG, "sent");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void receiveTask(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(sema, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            ESP_LOGI(TAG, "received");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    sema = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(sendTask, "send", 1024 * 3, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(receiveTask, "receive", 1024 * 3, NULL, 3, NULL, 1);
}
