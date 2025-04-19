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

esp_err_t create_dir(const char *path)
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

/*
    w  如果文件不存在 会创建 如果存在 会清空再写入
    a  如果文件不存在 会创建 如果存在 会追加写入
    r  只读 必须存在
    w+ 读写 存在则清空
    a+ 读写 从末尾写入 不存在会创建
    r+ 读写 覆盖
     */

esp_err_t write_file(const char *path, const char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "a");
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

esp_err_t read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];

    while (fgets(line, sizeof(line), f))
    {
        // 去除换行符
        char *pos = strchr(line, '\n');
        if (pos)
        {
            *pos = '\0';
        }

        ESP_LOGI(TAG, "Line: %s", line);
    }

    fclose(f);
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

esp_err_t create_file_and_folder(const char *file, const char *data, const char *folder)
{
    char path[128];
    if (snprintf(path, sizeof(path), "%s/%s", MOUNT_POINT, folder) >= sizeof(path))
    {
        ESP_LOGE(TAG, "Folder path too long");
        return ESP_FAIL;
    }

    if (create_dir(path) != ESP_OK)
        return ESP_FAIL;

    char full_path[128];
    if (snprintf(full_path, sizeof(full_path), "%s/%s", path, file) >= sizeof(full_path))
    {
        ESP_LOGE(TAG, "File path too long");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Full file path: %s", full_path);
    return write_file(full_path, data);
}

esp_err_t read_file_and_folder(const char *file, const char *data, const char *folder)
{
    char path[128];
    if (snprintf(path, sizeof(path), "%s/%s", MOUNT_POINT, folder) >= sizeof(path))
    {
        ESP_LOGE(TAG, "Folder path too long");
        return ESP_FAIL;
    }

    if (create_dir(path) != ESP_OK)
        return ESP_FAIL;

    char full_path[128];
    if (snprintf(full_path, sizeof(full_path), "%s/%s", path, file) >= sizeof(full_path))
    {
        ESP_LOGE(TAG, "File path too long");
        return ESP_FAIL;
    }

    return read_file(full_path);
}
