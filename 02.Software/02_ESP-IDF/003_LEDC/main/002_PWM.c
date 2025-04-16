#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define LED_NUM GPIO_NUM_2
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000

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
        .gpio_num = LED_NUM,
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

void pwmBlinkTask(void *param)
{
    gpio_simple_init(LED_NUM);
    timer_channel_simple_init();

    while (1)
    {
        
        for (int duty = 0; duty < (1 << LEDC_DUTY_RES); duty += 100)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        for (int duty = (1 << LEDC_DUTY_RES); duty > 0; duty -= 100)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(pwmBlinkTask, "pwm blink", 1024 * 5, NULL, 3, NULL, 1);
}