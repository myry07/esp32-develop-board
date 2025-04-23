#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_spi.h"
#include "esp_system.h"

#define EXAMPLE_MAX_CHAR_SIZE 64

#define PIN_NUM_MISO GPIO_NUM_5
#define PIN_NUM_MOSI GPIO_NUM_18
#define PIN_NUM_CLK GPIO_NUM_19
#define PIN_NUM_CS GPIO_NUM_23

#define MOUNT_POINT "/FAT"
#define DEFAULT_PATH "idf_mp3/"
#define RECORD_DOC "RECORD.TXT"

static sdmmc_card_t *sd_card = NULL;
static sdmmc_host_t host = SDSPI_HOST_DEFAULT();

const char *TAG = "SD";

extern TaskHandle_t i2sHandle;

int folder;
char folder_path[128];

int count_folders(char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Failed to open directory: %s", path);
        vTaskDelete(NULL);
        return -1;
    }

    struct dirent *entry;
    struct stat st;
    int folder_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        char full_path[300];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            // 排除 "." 和 ".."
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                ESP_LOGI(TAG, "Found folder: %s", entry->d_name);
                folder_count++;
            }
        }
    }

    closedir(dir);
    ESP_LOGI(TAG, "Total folders in %s: %d", path, folder_count);

    return folder_count;
}

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

esp_err_t write_file(const char *path, int num)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, "%d", num);
    fclose(f);
    ESP_LOGI(TAG, "Get Number: %d", num);

    xTaskNotify(i2sHandle, num, eSetValueWithOverwrite); // 通知给播放任务

    return ESP_OK;
}

int read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char buffer[16];
    if (fgets(buffer, sizeof(buffer), f) == NULL)
    {
        ESP_LOGE(TAG, "Failed to read the number");
        fclose(f);
        return -1;
    }

    fclose(f);
    int num = atoi(buffer); // 或者使用 strtol() 更安全
    return num;
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

    ESP_LOGI("SD", "All files played. System will restart.");

    vTaskDelay(pdMS_TO_TICKS(1000)); // 给点时间刷完串口日志

    esp_restart();
}


FILE *sd_i2s_open_file(const char *filename)
{
    /*  /FAT/idf_mp3/ */
    char path[128];
    if (snprintf(path, sizeof(path), "%s/%s", MOUNT_POINT, DEFAULT_PATH) >= sizeof(path))
    {
        ESP_LOGE(TAG, "File path too long");
        return NULL;
    }

    int max_folder = count_folders(path);

    /*  /FAT/idf_mp3/RECORD.TXT */
    if (snprintf(folder_path, sizeof(folder_path), "%s%s", path, RECORD_DOC) >= sizeof(folder_path))
    {
        ESP_LOGE(TAG, "File path too long");
        return NULL;
    }

    folder = read_file(folder_path);

    if (folder > max_folder) {
        folder = 1;
    }

    /*  /FAT/idf_mp3/1/test.pcm */
    char full_path[128];
    if (snprintf(full_path, sizeof(full_path), "%s%d/%s", path, folder, filename) >= sizeof(full_path))
    {
        ESP_LOGE(TAG, "File path too long");
        return NULL;
    }

    FILE *my_file = fopen(full_path, "r");
    if (my_file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file: %s", full_path);
        return NULL;
    }

    ESP_LOGI(TAG, "Opened file: %s", full_path);
    return my_file;
}