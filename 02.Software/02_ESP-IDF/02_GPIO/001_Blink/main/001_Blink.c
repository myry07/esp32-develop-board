#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_NUM GPIO_NUM_2

void gpio_simple_init(int num)
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1 << num),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};

    if (gpio_config(&led_config) == ESP_OK)
    {
        ESP_LOGI("GPIO", "Init");
    }
    else
    {
        ESP_LOGI("GPIO", "Error");
    }
}

void blinkTask(void *param)
{
    gpio_simple_init(LED_NUM);
    int gpio_level = 1;
    while (1)
    {
        gpio_set_level(LED_NUM, gpio_level);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_level = !gpio_level;
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(blinkTask, "blink", 1024 * 2, NULL, 3, NULL, 1);
}
