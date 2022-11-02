//
// Created by Augtons on 2022/10/22.
//

#include "ycg_blufi.h"

static const char *TAG = "BLUFI_EVENT";

void ycg_blufi_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
    /* actually, should post to blufi_task handle the procedure,
     * now, as a example, we do it more simply */
    switch (event) {
        case ESP_BLUFI_EVENT_INIT_FINISH: {
            YCG_BLUFI_INFO("Blufi 初始化成功");
            esp_blufi_adv_start();
            break;
        }
        case ESP_BLUFI_EVENT_DEINIT_FINISH: {
            YCG_BLUFI_INFO("Blufi 反初始化成功");
            break;
        }
        case ESP_BLUFI_EVENT_BLE_CONNECT: {
            YCG_BLUFI_INFO("Blufi 有新设备连接成功");

            ycg_system.blufi.device_connected = true;
            esp_blufi_adv_stop();
            blufi_security_init();
            break;
        }
        case ESP_BLUFI_EVENT_BLE_DISCONNECT: {
            YCG_BLUFI_INFO("Blufi 断开连接");
            ycg_system.blufi.device_connected = false;
            blufi_security_deinit();
            esp_blufi_adv_start();
            break;
        }
        case ESP_BLUFI_EVENT_SET_WIFI_OPMODE: {
            esp_err_t err = esp_wifi_set_mode(param->wifi_mode.op_mode);
            if (err == ESP_OK) {
                YCG_BLUFI_INFO("Blufi设置Wifi模式为 `%d` 成功", param->wifi_mode.op_mode);
            } else {
                YCG_BLUFI_ERROR("Blufi设置Wifi模式为 `%d` 失败，原因: %s", param->wifi_mode.op_mode,
                                esp_err_to_name(err));
            }
            break;
        }
        case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP: {
            YCG_BLUFI_INFO("Blufi请求连接到WiFi");
            esp_wifi_disconnect();
            esp_wifi_connect();
            break;
        }
        case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP: {
            YCG_BLUFI_INFO("Blufi请求断开WiFi连接");
            esp_wifi_disconnect();
            break;
        }
        case ESP_BLUFI_EVENT_REPORT_ERROR: {
            YCG_BLUFI_ERROR("Blufi报告错误, 错误代码为 %d\n", param->report_error.state);
            esp_blufi_send_error_info(param->report_error.state);
            break;
        }
        case ESP_BLUFI_EVENT_GET_WIFI_STATUS: {
            wifi_mode_t mode;
            esp_blufi_extra_info_t info = {0};
            esp_wifi_get_mode(&mode);

            EventBits_t bits = xEventGroupWaitBits(YCG_NETWORK_EVENTS, YCG_EVENT_NETWORK_WIFI_CONNECTED, false, true, 0);
            if (bits & YCG_EVENT_NETWORK_WIFI_CONNECTED) {
                memcpy(info.sta_bssid, ycg_system.network.sta_config.sta.bssid, 6);
                info.sta_bssid_set = true;

                info.sta_ssid = ycg_system.network.sta_config.sta.ssid;
                info.sta_ssid_len = (int)strlen((char*)ycg_system.network.sta_config.sta.ssid);
                esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
            } else {
                esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
            }
            YCG_BLUFI_INFO("Blufi获取WiFi状态");

            break;
        }
        case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE: {
            YCG_BLUFI_INFO("blufi close a gatt connection");
            esp_blufi_disconnect();
            break;
        }
        case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
            /* TODO */
            break;
        case ESP_BLUFI_EVENT_RECV_STA_BSSID: {
            memcpy(ycg_system.network.sta_config.sta.bssid, param->sta_bssid.bssid, 6);
            ycg_system.network.sta_config.sta.bssid_set = true;
            esp_wifi_set_config(WIFI_IF_STA, &ycg_system.network.sta_config);

            YCG_BLUFI_INFO("接收到BSSID:%s\n", ycg_system.network.sta_config.sta.bssid);
        }
            break;
        case ESP_BLUFI_EVENT_RECV_STA_SSID: {
            memset(ycg_system.network.sta_config.sta.ssid, 0, 32);
            strncpy((char *) ycg_system.network.sta_config.sta.ssid, (char *) param->sta_ssid.ssid,
                    param->sta_ssid.ssid_len);
            esp_wifi_set_config(WIFI_IF_STA, &ycg_system.network.sta_config);
            YCG_BLUFI_INFO("接收到SSID %s\n", ycg_system.network.sta_config.sta.ssid);
            break;
        }
        case ESP_BLUFI_EVENT_RECV_STA_PASSWD: {
            memset(ycg_system.network.sta_config.sta.password, 0, 64);
            strncpy((char *) ycg_system.network.sta_config.sta.password, (char *) param->sta_passwd.passwd,
                    param->sta_passwd.passwd_len);
            esp_wifi_set_config(WIFI_IF_STA, &ycg_system.network.sta_config);
            YCG_BLUFI_INFO("接收到密码 %s\n", ycg_system.network.sta_config.sta.password);
            break;
        }
        case ESP_BLUFI_EVENT_GET_WIFI_LIST:{
            wifi_scan_config_t scanConf = {
                .ssid = NULL,
                .bssid = NULL,
                .channel = 0,
                .show_hidden = false
            };
            esp_wifi_scan_start(&scanConf, true);
            break;
        }
        case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA: {
            YCG_BLUFI_INFO("接收到自定义数据，长度: %d\n", param->custom_data.data_len);
            esp_log_buffer_hex("Custom Data", param->custom_data.data, param->custom_data.data_len);
            break;
        }
        case ESP_BLUFI_EVENT_RECV_USERNAME:
            /* Not handle currently */
            break;
        case ESP_BLUFI_EVENT_RECV_CA_CERT:
            /* Not handle currently */
            break;
        case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
            /* Not handle currently */
            break;
        case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
            /* Not handle currently */
            break;
        case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
            /* Not handle currently */
            break;;
        case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
            /* Not handle currently */
            break;
        default:
            break;
    }
}