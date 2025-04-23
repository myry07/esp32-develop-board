#ifndef _SD_SPI_H
#define _SD_SPI_H

#include <stdio.h>
#include "esp_err.h"

esp_err_t create_dir(const char *path);
esp_err_t write_file(const char *path, int num);
int read_file(const char *path);
esp_err_t sd_spi_init(void);
esp_err_t sd_spi_deinit(void);
FILE *sd_i2s_open_file(const char *filename);

#endif