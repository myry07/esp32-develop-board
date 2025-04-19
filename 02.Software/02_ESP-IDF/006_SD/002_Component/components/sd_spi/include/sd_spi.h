#ifndef _SD_SPI_H
#define _SD_SPI_H


esp_err_t create_dir_if_not_exist(const char *path);
esp_err_t s_example_write_file(const char *path, char *data);
esp_err_t s_example_read_file(const char *path);
esp_err_t sd_spi_init(void);
esp_err_t sd_spi_deinit(void);
esp_err_t create_file_and_folder(const char *file, const char *data, const char *folder);

#endif