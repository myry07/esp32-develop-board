#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_NUM 2

SemaphoreHandle_t semaLED = NULL;

void gpio_simple_init()
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1 << LED_NUM),
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

void buttonTask(void *param)
{

}

void app_main(void)
{
    semaLED = xSemaphoreCreateBinary();
}
