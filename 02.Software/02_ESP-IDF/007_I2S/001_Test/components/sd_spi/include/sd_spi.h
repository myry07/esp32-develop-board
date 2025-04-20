#ifndef _SD_SPI_H
#define _SD_SPI_H

#define MOUNT_POINT "/FAT"

esp_err_t create_dir(const char *path);
esp_err_t write_file(const char *path, const char *data);
esp_err_t read_file(const char *path);
esp_err_t sd_spi_init(void);
esp_err_t sd_spi_deinit(void);
esp_err_t create_file_and_folder(const char *file, const char *data, const char *folder);
esp_err_t read_file_and_folder(const char *file, const char *data, const char *folder);
FILE *sd_i2s_open_file(const char *filename);

#endif