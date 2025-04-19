#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_spi.h"

#define EXAMPLE_MAX_CHAR_SIZE 64

static const char *TAG = "SD";

#define MOUNT_POINT "/FAT"

#define PIN_NUM_MISO GPIO_NUM_5
#define PIN_NUM_MOSI GPIO_NUM_18
#define PIN_NUM_CLK GPIO_NUM_19
#define PIN_NUM_CS GPIO_NUM_23

static sdmmc_card_t *sd_card = NULL;
static sdmmc_host_t host = SDSPI_HOST_DEFAULT();

esp_err_t create_dir_if_not_exist(const char *path)
{
    struct stat st;

    // 检查路径是否已存在
    if (stat(path, &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            ESP_LOGI(TAG, "Directory already exists: %s", path);
            return ESP_OK;
        }
        else
        {
            ESP_LOGE(TAG, "Path exists but is not a directory: %s", path);
            return ESP_FAIL;
        }
    }

    // 不存在，尝试创建
    int res = mkdir(path, 0777); // 全权限
    if (res == 0)
    {
        ESP_LOGI(TAG, "Directory created: %s", path);
        return ESP_OK;
    }
    else
    {
        ESP_LOGI(TAG, "Failed to create directory");
        return ESP_FAIL;
    }
}

esp_err_t s_example_write_file(const char *path, char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI(TAG, "File written");

    return ESP_OK;
}

esp_err_t s_example_read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    return ESP_OK;
}

esp_err_t sd_spi_init(void)
{
    esp_err_t ret;

    // 配置挂载参数
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 4 * 1024};

    // 初始化 SPI 总线
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE("SD", "Failed to initialize bus: %s", esp_err_to_name(ret));
        return ret;
    }

    // 配置 SD 卡设备
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    // 挂载文件系统
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &sd_card);
    if (ret != ESP_OK)
    {
        ESP_LOGE("SD", "Failed to mount SD card: %s", esp_err_to_name(ret));
        spi_bus_free(host.slot); // 别忘了释放
        return ret;
    }

    ESP_LOGI("SD", "SD card mounted at %s", MOUNT_POINT);
    sdmmc_card_print_info(stdout, sd_card);
    return ESP_OK;
}

esp_err_t sd_spi_deinit(void)
{
    if (sd_card)
    {
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, sd_card);
        ESP_LOGI("SD", "SD card unmounted");
        sd_card = NULL;
    }

    spi_bus_free(host.slot);
    ESP_LOGI("SD", "SPI bus freed");
    return ESP_OK;
}

esp_err_t create_file(const char *file, const char *data)
{
    esp_err_t ret = s_example_read_file(file);
    if (ret == ESP_OK)
    {
        ESP_LOGI("FILE", "File already exists: %s", file);
        return ESP_OK;
    }

    ESP_LOGW("FILE", "File not found, creating: %s", file);
    ret = s_example_write_file(file, (char *)data);
    if (ret != ESP_OK)
    {
        ESP_LOGE("FILE", "Failed to create file: %s", file);
    }

    return ret;
}

esp_err_t create_file_and_folder(const char *file, const char *data, const char *folder)
{
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", MOUNT_POINT, folder);

    esp_err_t ret = create_dir_if_not_exist(path);
    if (ret != ESP_OK)
        return ESP_FAIL;

    char full_path[128];
    snprintf(full_path, sizeof(full_path), "%s%s", path, file);
    return create_file(full_path, data);
}
