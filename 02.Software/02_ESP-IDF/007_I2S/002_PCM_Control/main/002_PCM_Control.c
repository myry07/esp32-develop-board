#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sd_spi.h"
#include "max98357.h"

void app_main(void)
{
    sd_spi_init();
    i2s_init();
    xTaskCreate(i2s_write_task, "i2s_example_write_task", 4096 * 3, (void *)"test.pcm", 5, NULL);
}