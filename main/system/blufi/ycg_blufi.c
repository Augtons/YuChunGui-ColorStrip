//
// Created by Augtons on 2022/10/22.
//

#include "ycg_blufi.h"

static const char *TAG = "YCG_BLUFI";

static esp_blufi_callbacks_t example_callbacks = {
    .event_cb = ycg_blufi_event_callback,
    .negotiate_data_handler = blufi_dh_negotiate_data_handler,
    .encrypt_func = blufi_aes_encrypt,
    .decrypt_func = blufi_aes_decrypt,
    .checksum_func = blufi_crc_checksum,
};

esp_err_t ycg_ble_controller_init()
{
    esp_err_t ret = ESP_OK;

    // 1. 释放CLASSIC_BT的内存，因为我们只用到了BLE
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    // 2. 初始化BT控制器
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("蓝牙控制器初始化失败");
        return ret;
    }
    // 3. 使能BT控制器
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("蓝牙控制器使能失败");
        return ret;
    }

    return ret;
}

esp_err_t ycg_bluedroid_init()
{
    esp_err_t ret = ESP_OK;

    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("Bluedroid初始化失败");
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("Bluedroid使能失败");
        return ret;
    }

    YCG_BLUFI_INFO("Bluedroid初始化成功! 地址："ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX(esp_bt_dev_get_address()));

    return ret;
}

esp_err_t ycg_blufi_init()
{
    esp_err_t ret = ESP_OK;

    ret = esp_blufi_register_callbacks(&example_callbacks);
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("Blufi回调注册失败");
        return ret;
    }

    ret = esp_ble_gap_register_callback(esp_blufi_gap_event_handler);
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("Blufi GAP Event 回调注册失败");
        return ret;
    }

    ret = esp_blufi_profile_init();
    if (ret != ESP_OK) {
        YCG_BLUFI_ERROR("Blufi Profile 初始化失败");
        return ret;
    }

    YCG_BLUFI_INFO("Blufi 版本: %04x", esp_blufi_get_version());

    return ret;
}