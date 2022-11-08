//
// Created by Augtons on 2022/10/10.
//

#include "system.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "tencent_cloud_app.h"
#include "data_model/data_config.c"

DeviceInfo deviceInfo;
Qcloud_IoT_Template *ioTTemplate;

SemaphoreHandle_t ycg_data_lock;

static const char *TAG = "MY_TENCENT_CLOUD_APP";
static DeviceProperty *properties[TOTAL_PROPERTY_COUNT] = {0};

static char report_json[512] = {0};

_Noreturn void tencent_cload_app(void *arg) {
    int rc = 0;
    ycg_data_lock = xSemaphoreCreateMutex();
    _init_data_template();

    HAL_GetDevInfo(&deviceInfo);

    /* Wait for Wi-Fi connecting. */
    xEventGroupWaitBits(YCG_NETWORK_EVENTS, YCG_EVENT_NETWORK_IP_GOT, false, true, portMAX_DELAY);

    TemplateInitParams params = DEFAULT_TEMPLATE_INIT_PARAMS;
    params.region = deviceInfo.region;
    params.product_id = deviceInfo.product_id;
    params.device_name = deviceInfo.device_name;
    params.device_secret = deviceInfo.device_secret;

    while (1) {
        void *rt = IOT_Template_Construct(&params, NULL);
        if (rt == NULL) {
            vTaskDelay(10000);
            ESP_LOGI("TAG", "创建MQTT客户端失败或连接失败，10秒后重试。");
            continue;
        } else {
            IOT_Template_Start_Yield_Thread(rt);
            ioTTemplate = rt;
            break;
        }
    }

    xTaskCreate(ycg_app, "ycg_app", 4096, NULL, 2, NULL);

    IOT_Template_Register_Property(ioTTemplate, &sg_DataTemplate[0].data_property, on_power_switch_state_change);
    IOT_Template_Register_Property(ioTTemplate, &sg_DataTemplate[1].data_property, on_brightness_change);
    IOT_Template_Register_Property(ioTTemplate, &sg_DataTemplate[2].data_property, on_color_change);
    IOT_Template_Register_Property(ioTTemplate, &sg_DataTemplate[3].data_property, NULL);
    IOT_Template_Register_Property(ioTTemplate, &sg_DataTemplate[4].data_property, NULL);

    while (1) {
        if (xSemaphoreTake(ycg_data_lock, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        properties[0] = &sg_DataTemplate[0].data_property;
        properties[1] = &sg_DataTemplate[1].data_property;
        properties[2] = &sg_DataTemplate[2].data_property;
        properties[3] = &sg_DataTemplate[3].data_property;
        properties[4] = &sg_DataTemplate[4].data_property;

        rc = IOT_Template_JSON_ConstructReportArray(ioTTemplate, report_json, sizeof(report_json), 5, properties);
        if (rc != QCLOUD_RET_SUCCESS) {
            ESP_LOGW(TAG, "构建汇报Json消息失败");
            goto continue_tag;
        }
        rc = IOT_Template_Report_Sync(ioTTemplate, report_json, sizeof(report_json), 1000);
        if (rc != QCLOUD_RET_SUCCESS) {
            ESP_LOGW(TAG, "汇报Json消息失败");
            goto continue_tag;
        }

    continue_tag:
        xSemaphoreGive(ycg_data_lock);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    vTaskDelete(NULL);
}
