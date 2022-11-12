#include <stdio.h>
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "system.h"
#include "system/blufi/ycg_blufi.h"
#include "system/wifi/ycg_wifi.h"

static const char* TAG = "MAIN";

_Noreturn void tencent_cload_app(void *arg);
_Noreturn void system_moniter(void *arg);

void init_devices() {
    // Create DHT11 Instance.
    YCG_CreateInstance( "DHT11",
        dht11_create(GPIO_NUM_6, &ycg_system.devices.dht11);
    );

    ws2812_rmt_init(RMT_CHANNEL_0, GPIO_NUM_7);
    YCG_CreateInstance( "WS2812",
        led_strip_create_ws2812(&(ws2812_config_t) {
            .rmt_channel = RMT_CHANNEL_0,
            .led_num = LED_NUMS
        }, &ycg_system.devices.ledStrip)
    )

    led_strip_clear(ycg_system.devices.ledStrip, pdMS_TO_TICKS(100));
}

void app_start() {
    xTaskCreate(tencent_cload_app, "tencent_cload_app", 4096, NULL, 1, NULL);
    //xTaskCreate(system_moniter, "system_moniter", 2048, NULL, 5, NULL);
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

    init_devices();

    ycg_wifi_init();
    esp_blufi_set_host_name("john-xie");

    ycg_ble_controller_init();
    ycg_bluedroid_init();
    ycg_blufi_init();

    app_start();
}

static char str[512] = {0};
_Noreturn void system_moniter(void *arg) {
    loop {
        vTaskList(str);
        printf("%s", str);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}