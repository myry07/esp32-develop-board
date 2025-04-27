#include <stdio.h>
#include "esp_partition.h"
#include "esp_log.h"
#include <string.h>
#include "esp_err.h"
#include "nvs_flash.h"

#define NAME_SPACE_WIFI1 "wifi1"
#define NAME_SPACE_WIFI2 "wifi2"

#define NVS_SSID_KEY "ssid"
#define NVS_PASSWORD_KEY "password"

static const char *TAG = "NVS";

void nvs_blob_read(const char *namespace, const char *key, void *buff, int maxlen)
{
    nvs_handle_t nvs_handle;
    size_t len = 0;
    nvs_open(namespace, NVS_READONLY, &nvs_handle);
    nvs_get_blob(nvs_handle, key, NULL, &len);

    if (len && len <= maxlen)
    {
        nvs_get_blob(nvs_handle, key, buff, &len);
    }
    nvs_close(nvs_handle);
}

void app_main(void)
{
    nvs_handle_t nvs_handle1;
    nvs_handle_t nvs_handle2;
    esp_err_t ret = nvs_flash_init();

    if (ret != ESP_OK)
    {
        nvs_flash_erase();
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    nvs_open(NAME_SPACE_WIFI1, NVS_READWRITE, &nvs_handle1);
    nvs_open(NAME_SPACE_WIFI2, NVS_READWRITE, &nvs_handle2);

    nvs_set_blob(nvs_handle1, NVS_SSID_KEY, "wifi_esp32-1", strlen("wifi_esp32-1"));
    nvs_set_blob(nvs_handle1, NVS_PASSWORD_KEY, "123456-1", strlen("123456-1"));

    nvs_set_blob(nvs_handle2, NVS_SSID_KEY, "wifi_esp32-2", strlen("wifi_esp32-2"));
    nvs_set_blob(nvs_handle2, NVS_PASSWORD_KEY, "123456-2", strlen("123456-2"));

    nvs_commit(nvs_handle1);
    nvs_commit(nvs_handle2);
    nvs_close(nvs_handle1);
    nvs_close(nvs_handle2);

    size_t len = 0;
    char read_buff[64];

    memset(read_buff, 0, sizeof(read_buff));
    nvs_blob_read(NAME_SPACE_WIFI1, NVS_SSID_KEY, read_buff, sizeof(read_buff));
    ESP_LOGI(TAG, "namespace: %s, key: %s -> value: %s", NAME_SPACE_WIFI1, NVS_SSID_KEY, read_buff);

    memset(read_buff, 0, sizeof(read_buff));
    nvs_blob_read(NAME_SPACE_WIFI1, NVS_PASSWORD_KEY, read_buff, sizeof(read_buff));
    ESP_LOGI(TAG, "namespace: %s, key: %s -> value: %s", NAME_SPACE_WIFI1, NVS_PASSWORD_KEY, read_buff);

    memset(read_buff, 0, sizeof(read_buff));
    nvs_blob_read(NAME_SPACE_WIFI2, NVS_SSID_KEY, read_buff, sizeof(read_buff));
    ESP_LOGI(TAG, "namespace: %s, key: %s -> value: %s", NAME_SPACE_WIFI2, NVS_SSID_KEY, read_buff);

    memset(read_buff, 0, sizeof(read_buff));
    nvs_blob_read(NAME_SPACE_WIFI2, NVS_PASSWORD_KEY, read_buff, sizeof(read_buff));
    ESP_LOGI(TAG, "namespace: %s, key: %s -> value: %s", NAME_SPACE_WIFI2, NVS_PASSWORD_KEY, read_buff);
}
