#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "wifi_manager/wifi_manager.h"
#include "blufi/ycg_blufi.h"

#include "data_config.c"

static const char* TAG = "MAIN";

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
    ycg_blufi_init();
}