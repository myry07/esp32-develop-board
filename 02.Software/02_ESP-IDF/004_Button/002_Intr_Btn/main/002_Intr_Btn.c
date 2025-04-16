#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_NUM 0
#define LED_NUM 2
#define TAGB "BUTTON"
#define TAGL "LED"

static volatile bool buttonReady = true;

TimerHandle_t debounce;
SemaphoreHandle_t sema = NULL;

void gpio_blink_init()
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1 << LED_NUM),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};

    if (gpio_config(&led_config) == ESP_OK)
    {
        ESP_LOGI(TAGL, "Init");
    }
    else
    {
        ESP_LOGI(TAGL, "Error");
    }
}

void gpio_button_init()
{
    gpio_config_t btn_config =
        {
            .pin_bit_mask = 1 << BUTTON_NUM,
            .intr_type = GPIO_INTR_NEGEDGE,
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_ENABLE,
        };

    if (gpio_config(&btn_config) == ESP_OK)
    {
        ESP_LOGI(TAGB, "Init");
    }
    else
    {
        ESP_LOGI(TAGB, "Error");
    }
}

void timerCallBack(void *param)
{
    buttonReady = true;
}

void IRAM_ATTR buttonISR(void *param)
{
    if (buttonReady)
    {
        buttonReady = false;
        xSemaphoreGiveFromISR(sema, NULL);
        xTimerStartFromISR(debounce, NULL);
    }
}

void ledTask(void *param)
{
    gpio_blink_init();
    gpio_button_init();
    bool ledstate = false;

    while (1)
    {
        if (xSemaphoreTake(sema, pdMS_TO_TICKS(3000)) == pdTRUE)
        {
            ledstate = !ledstate;
            if (ledstate)
            {
                gpio_set_level(LED_NUM, 1);
            }
            else
            {
                gpio_set_level(LED_NUM, 0);
            }
        }
    }
}

void app_main(void)
{
    sema = xSemaphoreCreateBinary();

    gpio_install_isr_service(0);                       // 安装 GPIO 中断服务，参数 0 表示默认配置
    gpio_isr_handler_add(BUTTON_NUM, buttonISR, NULL); // 给 BUTTON_NUM 注册 ISR 函数

    xTaskCreatePinnedToCore(ledTask, "led", 1024 * 4, NULL, 3, NULL, 1);
    debounce = xTimerCreate("debounce", pdMS_TO_TICKS(100), pdFALSE, (void *)0, timerCallBack);
}