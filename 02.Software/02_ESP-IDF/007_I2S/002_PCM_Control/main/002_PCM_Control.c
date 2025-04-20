#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sd_spi.h"
#include "max98357.h"
#include "button.h"
#include "esp_log.h"

TaskHandle_t i2sHandle = NULL;
volatile bool is_paused = false;

void short_press_cb(void)
{
    // is_paused = !is_paused;
    // if (!is_paused && i2sHandle != NULL)
    // {
    //     xTaskNotifyGive(i2sHandle); // 通知恢复
    // }
    // ESP_LOGI("PCM", "short press");
}

void long_press_cb(void)
{
    is_paused = !is_paused;
    xTaskNotifyGive(i2sHandle); // 通知

    ESP_LOGI("PCM", "long press is_paused: %d", is_paused);
}

void app_main(void)
{
    button_config_t cfg = {
        .gpio_num = 0,
        .active_level = 0,       // GPIO0 接地为按下
        .long_press_time = 1500, // 1500ms 判定为长按
        .short_cb = short_press_cb,
        .long_cb = long_press_cb,
    };

    button_event_set(&cfg);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    sd_spi_init();
    i2s_init();

    xTaskCreate(i2s_write_task, "i2s_example_write_task", 4096 * 3, (void *)"test.pcm", 5, &i2sHandle);
}