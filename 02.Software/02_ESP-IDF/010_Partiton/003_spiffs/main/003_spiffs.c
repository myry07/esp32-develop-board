#include <stdio.h>
#include "esp_partition.h"
#include "esp_log.h"
#include <string.h>
#include "esp_spiffs.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    esp_vfs_spiffs_conf_t cfg =
        {
            .base_path = "/spiffs",         // 挂载点
            .partition_label = NULL,        // 寻找 subtype的spiffs
            
            .max_files = 5,                 // 最大打开数
            .format_if_mount_failed = true // 挂载失败 格式化
        };

    esp_err_t ret = esp_vfs_spiffs_register(&cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE("SPIFFS", "spiffs mount fail!");
        return;
    }

    ret = esp_spiffs_check(cfg.partition_label); // 初始化时检查一下就行
    if (ret != ESP_OK)
    {
        ESP_LOGE("SPIFFS", "spiffs check error!");
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(cfg.partition_label, &total, &used); // 可用的与总的
    if (ret != ESP_OK)
    {
        ESP_LOGE("SPIFFS", "spiffs info error!");
        return;
    }

    ESP_LOGI("SPIFFS", "total size: %d, used size %d", total, used);

    if (used > total)
    {
        ESP_LOGE("SPIFFS", "spiffs used > total!");
        return;
    }

    FILE *f = fopen("/spiffs/hello.txt", "w"); // w 打开
    if (f == NULL)
    {
        ESP_LOGE("SPIFFS", "Filed to open!");
        return;
    }

    fprintf(f, "Hello World\n"); // 写入
    fclose(f);

    vTaskDelay(pdMS_TO_TICKS(1000));

    f = fopen("/spiffs/hello.txt", "r"); // r 打开
    if (f == NULL)
    {
        ESP_LOGE("SPIFFS", "Filed to open!");
        return;
    }

    char line[64];
    fgets(line, sizeof(line), f); // 读取
    fclose(f);

    char *pos = strchr(line, '\n'); // 删除换行符
    if (pos)
    {
        *pos = 0;
    }

    ESP_LOGI("SPIFFS", "read str: %s", line);
    esp_vfs_spiffs_unregister(cfg.partition_label);
}
