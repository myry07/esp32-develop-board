#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_NUM 0
#define TAG "BUTTON"

#define LONG_PRESS_TIME 3000
#define STABLE 100

typedef enum
{
    IDLE,
    DEBOUNCE,
    PRESSED,
    LONG_PRESS
} KeyState;

static KeyState key_state = IDLE;
static unsigned short key_timer = 0;

void check_key_task(void *param)
{
    while (1)
    {
        
        bool key_down = gpio_get_level(BUTTON_NUM);

        switch (key_state)
        {
        case IDLE:
            if (key_down)
            {
                key_state = DEBOUNCE;
                key_timer = 0;
            }
            break;

        case DEBOUNCE:
            if (key_down)
            {
                key_timer++;
                if (key_timer >= STABLE)
                {
                    key_state = PRESSED;
                    key_timer = 0;
                    ESP_LOGI(TAG, "PRESSED");
                }
            }
            else
            {
                key_state = IDLE; // 抖动返回
            }
            break;

        case PRESSED:
            if (key_down)
            {
                key_timer++;
                if (key_timer >= LONG_PRESS_TIME)
                {
                    key_state = LONG_PRESS;
                    ESP_LOGI(TAG, "LONG PRESSED");
                }
            }
            else
            {
                key_state = IDLE;
                ESP_LOGI(TAG, "SHORT PRESSED");
            }
            break;

        case LONG_PRESS:
            if (!key_down)
            {
                key_state = IDLE;
            }
            break;

        default:
            key_state = IDLE;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms扫描一次
    }
}

void app_main(void)
{
    gpio_config_t btn_config = {
        .pin_bit_mask = 1 << BUTTON_NUM,
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn_config));

    xTaskCreate(check_key_task, "key_scan", 2048, NULL, 5, NULL);
}
