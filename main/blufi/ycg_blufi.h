//
// Created by Augtons on 2022/10/7.
//

#ifndef YUCHUNGUI_COLORSTRIP2_YCG_BLUFI_H
#define YUCHUNGUI_COLORSTRIP2_YCG_BLUFI_H

#include "blufi_example.h"

struct ycg_blufi_obj {
    bool gl_sta_connected;
    bool ble_is_connected;
    uint8_t gl_sta_bssid[6];
    uint8_t gl_sta_ssid[32];
    int gl_sta_ssid_len;
    wifi_sta_list_t gl_sta_list;

    wifi_config_t sta_config;
    wifi_config_t ap_config;
};

typedef struct ycg_blufi_obj ycg_blufi_obj_t;

extern ycg_blufi_obj_t ycg_blufi_obj;

int softap_get_current_connection_number(void);

void ycg_blufi_init();

#endif //YUCHUNGUI_COLORSTRIP2_YCG_BLUFI_H
