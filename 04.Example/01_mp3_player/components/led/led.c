#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "led.h"

#define LED_GPIO GPIO_NUM_2
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000
#define MAX_DUTY ((1 << LEDC_DUTY_RES) - 1)

void led_simple_init()
{
    gpio_config_t led_config = {
        .pin_bit_mask = (1 << LED_GPIO),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&led_config);
}

void timer_channel_simple_init()
{
    ledc_timer_config_t led_t_config = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    ledc_timer_config(&led_t_config);

    ledc_channel_config_t led_c_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .gpio_num = LED_GPIO,
        .duty = 0,
        .intr_type = LEDC_INTR_DISABLE};

    ledc_channel_config(&led_c_config);
}

void blink_led()
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (1 << LEDC_DUTY_RES) - 1);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void sleep_led()
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void pwm_breath_task(void *param)
{
    while (1)
    {
        // 渐亮
        for (int duty = 0; duty < (1 << LEDC_DUTY_RES); duty += 100)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        // 渐灭
        for (int duty = (1 << LEDC_DUTY_RES); duty > 0; duty -= 100)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        vTaskDelay(pdMS_TO_TICKS(200));  // 每次循环后的延时
    }
}
