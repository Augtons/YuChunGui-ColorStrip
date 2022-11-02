//
// Created by Augtons on 2022/10/22.
//

#ifndef _YCG_WIFI_H
#define _YCG_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "system/system.h"

#define YCG_WIFI_INFO(format, args...)   ESP_LOGI(TAG, format, ##args)
#define YCG_WIFI_WARN(format, args...)   ESP_LOGW(TAG, format, ##args)
#define YCG_WIFI_ERROR(format, args...)  ESP_LOGE(TAG, format, ##args) \

void ycg_wifi_init();

#ifdef __cplusplus
}
#endif

#endif //_YCG_WIFI_H
