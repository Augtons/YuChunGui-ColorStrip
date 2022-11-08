//
// Created by Augtons on 2022/10/10.
//

#ifndef YUCHUNGUI_COLORSTRIP2_TENCENT_CLOUD_APP_H
#define YUCHUNGUI_COLORSTRIP2_TENCENT_CLOUD_APP_H

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "qcloud_iot_export_data_template.h"
#include "data_template_client.h"
#include "data_template_action.h"

typedef struct _ProductDataDefine {
    TYPE_DEF_TEMPLATE_BOOL m_power_switch;
    TYPE_DEF_TEMPLATE_INT m_brightness;
    TYPE_DEF_TEMPLATE_STRING m_color_value[2048+1];
    TYPE_DEF_TEMPLATE_FLOAT m_temp;
    TYPE_DEF_TEMPLATE_INT m_humi;
} ProductDataDefine;

extern DeviceInfo               deviceInfo;
extern Qcloud_IoT_Template     *ioTTemplate;
extern ProductDataDefine        sg_ProductData;
extern SemaphoreHandle_t        ycg_data_lock;

void ycg_app(void *arg);

void on_power_switch_state_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty);
void on_brightness_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty);
void on_color_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty);

#endif //YUCHUNGUI_COLORSTRIP2_TENCENT_CLOUD_APP_H
