//
// Created by Augtons on 2022/10/22.
//

#ifndef _YCG_BLUFI_H
#define _YCG_BLUFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#define YCG_BLUFI_INFO(format, args...)  ESP_LOGI(TAG, format, ##args)
#define YCG_BLUFI_WARN(format, args...)  ESP_LOGW(TAG, format, ##args)
#define YCG_BLUFI_ERROR(format, args...)  ESP_LOGE(TAG, format, ##args)

void ycg_blufi_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);

esp_err_t blufi_security_init(void);
void blufi_security_deinit(void);

void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free);
int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len);
int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len);
uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len);

esp_err_t ycg_ble_controller_init();
esp_err_t ycg_bluedroid_init();
esp_err_t ycg_blufi_init();

void on_blufi_custom_data_receive(uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif //_YCG_BLUFI_H
