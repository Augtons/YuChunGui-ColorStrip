//
// Created by Augtons on 2022/10/10.
//

#include "system.h"

#include "esp_log.h"

#include "tencent_cloud_app.h"
#include "tencent_cloud/include/exports/qcloud_iot_export_data_template.h"
#include "tencent_cloud/include/qcloud_iot_import.h"
#include "tencent_cloud/include/qcloud_iot_export.h"

#include "data_model/data_config.c"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ycg_blufi.h"

static DeviceInfo deviceInfo;

void tencent_cload_app() {
    _init_data_template();

    HAL_GetDevInfo(&deviceInfo);

    while (ycg_system.network.wifi_connected) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    TemplateInitParams params = DEFAULT_TEMPLATE_INIT_PARAMS;
    params.region = deviceInfo.region;
    params.product_id = deviceInfo.product_id;
    params.device_name = deviceInfo.device_name;
    params.device_secret = deviceInfo.device_secret;

    void *rt = IOT_Template_Construct(&params, NULL);
    IOT_Template_Start_Yield_Thread(rt);

    ESP_LOGI("TAG", "%s", (rt != NULL ? "连接成功" : "连接失败"));

    vTaskDelete(NULL);

}