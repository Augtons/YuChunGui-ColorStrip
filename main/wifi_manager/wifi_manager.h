//
// Created by Augtons on 2022/10/7.
//

#ifndef YUCHUNGUI_COLORSTRIP2_WIFI_MANAGER_H
#define YUCHUNGUI_COLORSTRIP2_WIFI_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

struct ycg_wifi_manager_t {
    esp_netif_t *netif_sta;
    esp_netif_t *netif_ap;

    esp_event_handler_instance_t wifi_event_handler_ins;
    esp_event_handler_instance_t ip_event_handler_ins;
};

typedef struct ycg_wifi_manager_t ycg_wifi_manager_t;

extern ycg_wifi_manager_t ycg_wifi_manager;

void ycg_wifi_init();

#endif //YUCHUNGUI_COLORSTRIP2_WIFI_MANAGER_H
