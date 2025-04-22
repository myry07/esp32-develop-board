#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "st7789_driver.h"

#define SPI_MOSI GPIO_NUM_18
#define SPI_CLK GPIO_NUM_19
#define LCD_CS GPIO_NUM_22
#define LCD_DC GPIO_NUM_26
#define LCD_RST GPIO_NUM_27
#define LCD_BLK GPIO_NUM_2
#define SPI_FREQENCY 40000000
#define LCD_WIDTH 240
#define LCD_HEIGHT 280

void lcd_done_cb(void *user_ctx)
{
}

void app_main(void)
{
    st7789_cfg_t cfg = {
        .mosi = SPI_MOSI,
        .clk = SPI_CLK,
        .cs = LCD_CS,
        .dc = LCD_DC,
        .rst = LCD_RST,
        .bl = LCD_BLK, // 如果你接了背光控制引脚就写对应GPIO号
        .spi_fre = SPI_FREQENCY,
        .width = LCD_WIDTH,
        .height = LCD_HEIGHT,
        .spin = 0, // 旋转方向：0~3
        .done_cb = lcd_done_cb,
        .cb_param = NULL,
    };

    st7789_driver_hw_init(&cfg);
    st7789_lcd_backlight(true);  // 打开背光

    // 分配一块白色缓冲区
    size_t buffer_size = LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t);
    uint16_t *buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA); // DMA可用
    if (buffer == NULL) {
        ESP_LOGE("APP", "Failed to allocate buffer");
        return;
    }

    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        buffer[i] = 0xFFFF;  // 全部填白
    }

    // 刷新整块屏幕
    st7789_flush(0, LCD_WIDTH, 0 + 20, LCD_HEIGHT + 20, buffer);

    ESP_LOGI("APP", "LCD should now show white screen");
}