#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_NUM 0
#define TAG "BUTTON"

void gpio_input_init()
{
    gpio_config_t btn_config =
        {
            .pin_bit_mask = 1 << BUTTON_NUM,
            .intr_type = GPIO_INTR_DISABLE,
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_ENABLE,
        };

    if (gpio_config(&btn_config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Init");
    }
    else
    {
        ESP_LOGI(TAG, "Error");
    }
}

void buttonTask(void *param)
{
    gpio_input_init();
    while (1)
    {
        if (gpio_get_level(BUTTON_NUM) == 0)
        {
            ESP_LOGI(TAG, "Button pressed!");
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(buttonTask, "button", 1024 * 4, NULL, 3, NULL, 1);
}
