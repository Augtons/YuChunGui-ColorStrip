#include <stdio.h>
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "system.h"
#include "system/blufi/ycg_blufi.h"
#include "system/wifi/ycg_wifi.h"

static const char* TAG = "MAIN";

void tencent_cload_app();

void app_start() {
    xTaskCreate(tencent_cload_app, "tencent_cload_app", 4096, NULL, 1, NULL);
    ESP_LOGI(TAG, "应用启动成功");
}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ycg_wifi_init();
    esp_blufi_set_host_name("john-xie");

    ycg_ble_controller_init();
    ycg_bluedroid_init();
    ycg_blufi_init();

    app_start();
}