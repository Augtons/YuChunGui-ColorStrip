//
// Created by Augtons on 2022/10/22.
//

#ifndef _YCG_SYSTEM_H
#define _YCG_SYSTEM_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_blufi.h"

struct system_t {
    // network
    struct network_t {
        bool wifi_inited;
        bool wifi_connected;

        esp_event_handler_instance_t wifi_event_handler_ins;
        esp_event_handler_instance_t ip_event_handler_ins;

        esp_netif_t *netif_sta;
        wifi_config_t sta_config;


    }network;

    // blufi
    struct blufi_t {
        bool device_connected;

    }blufi;

};

extern struct system_t ycg_system;

#endif //_YCG_SYSTEM_H
