#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "button.h"

#define LED_GPIO 2
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000
#define MAX_DUTY ((1 << LEDC_DUTY_RES) - 1)

static TaskHandle_t led_task_handle = NULL;

typedef enum
{
    LED_IDLE,
    LED_BLINK_ONCE,
    LED_PWM_LOOP
} led_mode_t;

void led_simple_init()
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1 << LED_GPIO),
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

void timer_channel_simple_init()
{
    ledc_timer_config_t led_t_config = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    if (ledc_timer_config(&led_t_config) == ESP_OK)
    {
        ESP_LOGI("Timer", "Init");
    }
    else
    {
        ESP_LOGI("Timer", "Error");
    }

    ledc_channel_config_t led_c_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .gpio_num = LED_GPIO,
        .duty = 0,
        .intr_type = LEDC_INTR_DISABLE};

    if (ledc_channel_config(&led_c_config) == ESP_OK)
    {
        ESP_LOGI("Channel", "Init");
    }
    else
    {
        ESP_LOGI("Channel", "Error");
    }
}

void short_press_cb(void)
{
    // value = 1
    xTaskNotify(led_task_handle, 1, eSetValueWithOverwrite);
}

void long_press_cb(void)
{
    // value = 2
    xTaskNotify(led_task_handle, 2, eSetValueWithOverwrite);
}

void led_task(void *arg)
{
    uint32_t notification_value;
    led_mode_t mode = LED_IDLE;

    while (1)
    {
        if (xTaskNotifyWait(0, 0, &notification_value, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            if (notification_value == 1)
            {
                mode = LED_BLINK_ONCE;
                ESP_LOGI("LED", "Short press: blink once");
            }
            else if (notification_value == 2)
            {
                mode = LED_PWM_LOOP;
                ESP_LOGI("LED", "Long press: start PWM loop");
            }
        }

        switch (mode)
        {
        case LED_BLINK_ONCE:
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (1 << LEDC_DUTY_RES) - 1);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(300));
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            mode = LED_IDLE;
            break;

        case LED_PWM_LOOP:
            for (int duty = 0; duty < (1 << LEDC_DUTY_RES); duty += 100)
            {
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            for (int duty = (1 << LEDC_DUTY_RES); duty > 0; duty -= 100)
            {
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            break;

        case LED_IDLE:
        default:
            break;
        }
    }
}

void app_main(void)
{
    led_simple_init();
    timer_channel_simple_init();

    xTaskCreate(led_task, "led_task", 2048 * 4, NULL, 5, &led_task_handle);

    button_config_t cfg = {
        .gpio_num = 0,
        .active_level = 0,
        .long_press_time = 1500, // ms
        .short_cb = short_press_cb,
        .long_cb = long_press_cb,
    };

    button_event_set(&cfg);
}