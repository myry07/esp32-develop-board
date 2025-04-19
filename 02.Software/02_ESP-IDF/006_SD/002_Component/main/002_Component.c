#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sd_spi.h"

void app_main(void)
{
    const char *file = "/goodbye.txt";
    const char *folder = "IDF_SPI";
    const char *data = "Good Bye hapless!";
    sd_spi_init();
    create_file_and_folder(file, data, folder);
    sd_spi_deinit();
}
