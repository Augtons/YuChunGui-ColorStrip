//
// Created by Augtons on 2022/10/7.
//

#include "ycg_blufi.h"
#include "wifi_manager.h"

ycg_wifi_manager_t ycg_wifi_manager = {0};

static const char* TAG = "YCG_WIFI";

static void ycg_wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    wifi_event_sta_connected_t *event;
    wifi_mode_t mode;

    switch (event_id) {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ycg_blufi_obj.gl_sta_connected = true;
            event = (wifi_event_sta_connected_t*) event_data;
            memcpy(ycg_blufi_obj.gl_sta_bssid, event->bssid, 6);
            memcpy(ycg_blufi_obj.gl_sta_ssid, event->ssid, event->ssid_len);
            ycg_blufi_obj.gl_sta_ssid_len = event->ssid_len;
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
            ycg_blufi_obj.gl_sta_connected = false;
            memset(ycg_blufi_obj.gl_sta_ssid, 0, 32);
            memset(ycg_blufi_obj.gl_sta_bssid, 0, 6);
            ycg_blufi_obj.gl_sta_ssid_len = 0;
            esp_wifi_connect();

            break;
        case WIFI_EVENT_AP_START:
            esp_wifi_get_mode(&mode);

            /* TODO: get config or information of softap, then set to report extra_info */
            if (ycg_blufi_obj.ble_is_connected == true) {
                if (ycg_blufi_obj.gl_sta_connected) {
                    esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, softap_get_current_connection_number(), NULL);
                } else {
                    esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, softap_get_current_connection_number(), NULL);
                }
            } else {
                BLUFI_INFO("BLUFI BLE is not connected yet\n");
            }
            break;
        case WIFI_EVENT_SCAN_DONE: {
            uint16_t apCount = 0;
            esp_wifi_scan_get_ap_num(&apCount);
            if (apCount == 0) {
                BLUFI_INFO("Nothing AP found");
                break;
            }
            wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
            if (!ap_list) {
                BLUFI_ERROR("malloc error, ap_list is NULL");
                break;
            }
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, ap_list));
            esp_blufi_ap_record_t * blufi_ap_list = (esp_blufi_ap_record_t *)malloc(apCount * sizeof(esp_blufi_ap_record_t));
            if (!blufi_ap_list) {
                if (ap_list) {
                    free(ap_list);
                }
                BLUFI_ERROR("malloc error, blufi_ap_list is NULL");
                break;
            }
            for (int i = 0; i < apCount; ++i)
            {
                blufi_ap_list[i].rssi = ap_list[i].rssi;
                memcpy(blufi_ap_list[i].ssid, ap_list[i].ssid, sizeof(ap_list[i].ssid));
            }

            if (ycg_blufi_obj.ble_is_connected == true) {
                esp_blufi_send_wifi_list(apCount, blufi_ap_list);
            } else {
                BLUFI_INFO("BLUFI BLE is not connected yet\n");
            }

            esp_wifi_scan_stop();
            free(ap_list);
            free(blufi_ap_list);
            break;
        }
        case WIFI_EVENT_AP_STACONNECTED: {
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            BLUFI_INFO("station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
            break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED: {
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            BLUFI_INFO("station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
            break;
        }

        default:
            break;
    }
    return;
}

static void ycg_ip_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    wifi_mode_t mode;

    switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {
            esp_blufi_extra_info_t info;

            //xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

            esp_wifi_get_mode(&mode);

            memset(&info, 0, sizeof(esp_blufi_extra_info_t));
            memcpy(info.sta_bssid, ycg_blufi_obj.gl_sta_bssid, 6);
            info.sta_bssid_set = true;
            info.sta_ssid = ycg_blufi_obj.gl_sta_ssid;
            info.sta_ssid_len = ycg_blufi_obj.gl_sta_ssid_len;
            if (ycg_blufi_obj.ble_is_connected == true) {
                esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, softap_get_current_connection_number(), &info);
            } else {
                BLUFI_INFO("BLUFI BLE is not connected yet\n");
            }
            break;
        }
        default:
            break;
    }
    return;
}

void ycg_wifi_init() {
    if (ycg_wifi_manager.wifi_inited == true) {
        ESP_LOGW(TAG, "Wifi has been initialized");
        return;
    }
    ycg_wifi_manager.wifi_inited = true;

    if (ycg_wifi_manager.netif_sta == NULL) {
        ycg_wifi_manager.netif_sta = esp_netif_create_default_wifi_sta();
        assert(ycg_wifi_manager.netif_sta);
    }

    if (ycg_wifi_manager.netif_ap == NULL) {
        ycg_wifi_manager.netif_ap = esp_netif_create_default_wifi_ap();
        assert(ycg_wifi_manager.netif_ap);
    }

    if (ycg_wifi_manager.wifi_event_handler_ins == NULL) {
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(
                WIFI_EVENT,
                ESP_EVENT_ANY_ID,
                ycg_wifi_event_handler,
                NULL,
                &ycg_wifi_manager.wifi_event_handler_ins
            )
        );
    }

    if (ycg_wifi_manager.ip_event_handler_ins) {
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(
                IP_EVENT,
                ESP_EVENT_ANY_ID,
                ycg_ip_event_handler,
                NULL,
                &ycg_wifi_manager.ip_event_handler_ins
            )
        );
    }

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}