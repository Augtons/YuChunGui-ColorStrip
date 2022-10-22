//
// Created by Augtons on 2022/10/22.
//

#include "ycg_wifi.h"

static const char *TAG = "YCG_WIFI";

static void ycg_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data) {

    switch (event_id) {
        case WIFI_EVENT_STA_START: {
            esp_wifi_connect();
            break;
        }
        case WIFI_EVENT_STA_CONNECTED: {
            wifi_event_sta_connected_t *event = event_data;
            strncpy((char*)ycg_system.network.sta_config.sta.ssid, (const char*)event->ssid, 32);
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED: {
            /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
            ycg_system.network.wifi_connected = false;

            esp_wifi_connect();
            break;
        }
        case WIFI_EVENT_SCAN_DONE: {
            uint16_t ap_num = 0;
            esp_wifi_scan_get_ap_num(&ap_num);

            wifi_ap_record_t *ap_list = NULL;
            esp_blufi_ap_record_t *blufi_ap_list = NULL;

            ap_list = calloc(sizeof(wifi_ap_record_t), ap_num);
            if (ap_list == NULL) {
                YCG_WIFI_ERROR("Malloc ap_list failed");
                goto finish;
            }
            if(esp_wifi_scan_get_ap_records(&ap_num, ap_list) != ESP_OK) {
                goto finish;
            }

            blufi_ap_list = calloc(sizeof(esp_blufi_ap_record_t), ap_num);
            if (ap_list == NULL) {
                YCG_WIFI_ERROR("Malloc blufi_ap_list failed");
                goto finish;
            }

            for (int i = 0; i < ap_num; ++i) {
                blufi_ap_list[i].rssi = ap_list[i].rssi;
                memcpy(blufi_ap_list[i].ssid, ap_list[i].ssid, sizeof(ap_list[i].ssid));
            }

            if (ycg_system.blufi.device_connected == true) {
                esp_blufi_send_wifi_list(ap_num, blufi_ap_list);
            }
        finish:
            free(ap_list);
            free(blufi_ap_list);
            esp_wifi_scan_stop();
            break;
        }

        default:
            break;
    }
}

static void ycg_ip_event_handler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data) {

    switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {
            wifi_mode_t mode;
            esp_blufi_extra_info_t info = {0};

            esp_wifi_get_mode(&mode);

            memcpy(info.sta_bssid, ycg_system.network.sta_config.sta.bssid, 6);
            info.sta_bssid_set = true;
            info.sta_ssid = ycg_system.network.sta_config.sta.ssid;
            info.sta_ssid_len = (int)strlen((char*)ycg_system.network.sta_config.sta.ssid);
            if (ycg_system.blufi.device_connected == true) {
                esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
            }
            ycg_system.network.wifi_connected = true;
            break;
        }
        default:
            break;
    }
}

void ycg_wifi_init() {
    if (ycg_system.network.wifi_inited == true) {
        ESP_LOGW(TAG, "Wifi has been initialized.");
        return;
    }

    YCG_WIFI_INFO("WiFi Config: %s(%s)", ycg_system.network.sta_config.sta.ssid, ycg_system.network.sta_config.sta.password);

    ycg_system.network.wifi_inited = true;

    if (ycg_system.network.netif_sta == NULL) {
        ycg_system.network.netif_sta = esp_netif_create_default_wifi_sta();
        assert(ycg_system.network.netif_sta);
    }

    if (ycg_system.network.wifi_event_handler_ins == NULL) {
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(
                WIFI_EVENT,
                ESP_EVENT_ANY_ID,
                ycg_wifi_event_handler,
                NULL,
                &ycg_system.network.wifi_event_handler_ins
            )
        );
    }

    if (ycg_system.network.ip_event_handler_ins == NULL) {
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(
                IP_EVENT,
                ESP_EVENT_ANY_ID,
                ycg_ip_event_handler,
                NULL,
                &ycg_system.network.ip_event_handler_ins
            )
        );
    }

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}