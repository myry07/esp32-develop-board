#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sd_spi.h"
#include "max98357.h"
#include "button.h"
#include "led.h"
#include "esp_log.h"

TaskHandle_t i2sHandle = NULL;

void app_main(void)
{
    led_simple_init();
    timer_channel_simple_init();
    button_init();
    sd_spi_init();
    i2s_init();
}