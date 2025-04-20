#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "sd_spi.h"
#include "esp_log.h"
#include <string.h>
#include "max98357.h"
#include "button.h"

#define I2S_BLCK GPIO_NUM_12      // I2S bit clock io number
#define I2S_WS GPIO_NUM_13        // I2S word select io number
#define I2S_DOUT GPIO_NUM_14      // I2S data out io number
#define I2S_DIN I2S_PIN_NO_CHANGE // I2S data in io number

#define SAMPLE_RATE (44100)
#define BUFF_SIZE 1024

extern volatile bool is_paused;
extern TaskHandle_t i2sHandle;

void i2s_write_task(void *param)
{
    char *f = (char *)param;

    FILE *my_file = sd_i2s_open_file(f);
    if (!my_file)
    {
        ESP_LOGE("I2S", "Failed to open file");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI("I2S", "Open the File");
    char line[BUFF_SIZE];

    while (!feof(my_file))
    {
        if (is_paused) {
            ESP_LOGI("I2S", "Paused");
            i2s_stop(I2S_NUM_0);  // 停止数据输出
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待继续
            i2s_start(I2S_NUM_0); // 恢复 I2S
        }

        // ESP_LOGI("I2S", "Playing");
        size_t len = fread(line, sizeof(char), sizeof(line), my_file);
        size_t bytes_written;
        i2s_write(I2S_NUM_0, line, len, &bytes_written, portMAX_DELAY);
    }

    fclose(my_file);
    ESP_LOGI("I2S", "Playback finished");
    vTaskDelete(NULL);
}

void i2s_init(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,                   // 必须与PCM一致
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 对应pcm_s16le
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // 单声道的正确设置
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .intr_alloc_flags = 0,
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BLCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_DIN};

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}
