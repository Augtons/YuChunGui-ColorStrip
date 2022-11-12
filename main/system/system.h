//
// Created by Augtons on 2022/10/22.
//

#ifndef _YCG_SYSTEM_H
#define _YCG_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_blufi.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "ycg_wifi.h"
#include "ycg_blufi.h"

#include "dht11.h"
#include "led_strip_impl_ws2812.h"
#include "led_strip.h"

struct system_t {
    // network
    struct network_t {
        bool wifi_inited;
        EventGroupHandle_t event;

        esp_event_handler_instance_t wifi_event_handler_ins;
        esp_event_handler_instance_t ip_event_handler_ins;

        esp_netif_t *netif_sta;
        wifi_config_t sta_config;

    } network;

    // blufi
    struct blufi_t {
        bool device_connected;

        EventGroupHandle_t event;

    } blufi;

    //dev
    struct devices_t {
        led_strip_t *ledStrip;
        dht11_handle_t dht11;
    } devices;

};

extern struct system_t ycg_system;

/* Device */
#define LED_NUMS (60)

#define YCG_CreateInstance(name, code)                              \
{                                                                   \
    esp_err_t __err = code;                                                         \
    if (__err == ESP_OK) {                                                \
        ESP_LOGI(TAG, "创建" #name "实例成功");                               \
    } else {                                                            \
        ESP_LOGE(TAG, "创建" #name "示例失败, 错误: %s", esp_err_to_name(__err)); \
    }                                                                   \
}

/* YCG Network events */
#define YCG_NETWORK_EVENTS                  (ycg_system.network.event)

#define YCG_EVENT_NETWORK_WIFI_CONNECTED    BIT(0)
#define YCG_EVENT_NETWORK_IP_GOT            BIT(1)

/* YCG Blufi events */
#define YCG_BLUFI_EVENTS                (ycg_system.blufi.event)

/* Language extensions*/
#define loop for(;;)

#define synchronized(lock, block)  if(xSemaphoreTake((lock), portMAX_DELAY)) { \
    block;                                                                     \
    xSemaphoreGive((lock));                                                    \
}

#ifdef __cplusplus
}
#endif

#endif //_YCG_SYSTEM_H
