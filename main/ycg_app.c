//
// Created by augtons on 22-11-8.
//

#include "system.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "tencent_cloud_app.h"
#include "dht11.h"
#include "led_strip.h"
#include "led_strip_impl_ws2812.h"

static const char *TAG = "YCG_APP";

static dht11_handle_t dht11;
static led_strip_t *ledStrip;

#define YCG_CreateInstance(name, code)                              \
{                                                                   \
    esp_err_t __err = code;                                                         \
    if (__err == ESP_OK) {                                                \
        ESP_LOGI(TAG, "创建" #name "实例成功");                               \
    } else {                                                            \
        ESP_LOGE(TAG, "创建" #name "示例失败, 错误: %s", esp_err_to_name(__err)); \
    }                                                                   \
}

_Noreturn void ycg_app(void *arg) {
    esp_err_t err = ESP_OK;
    // Create DHT11 Instance.
    YCG_CreateInstance( "DHT11",
        dht11_create(GPIO_NUM_6, &dht11)
    );

    ws2812_rmt_init(RMT_CHANNEL_0, GPIO_NUM_7);
    YCG_CreateInstance( "WS2812",
        led_strip_create_ws2812(&(ws2812_config_t) {
            .rmt_channel = RMT_CHANNEL_0,
            .led_num = 60
        }, &ledStrip)
    )

    led_strip_set_color(ledStrip, 6, 0, 128, 128);
    led_strip_refresh(ledStrip, pdMS_TO_TICKS(200));

    float temp = 0, humi = 0;

    while (1) {
        dht11_read(dht11, &temp, &humi);
        if (xSemaphoreTake(ycg_data_lock, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        sg_ProductData.m_temp = temp;
        sg_ProductData.m_humi = (int)humi;
        xSemaphoreGive(ycg_data_lock);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void on_power_switch_state_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                                  DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "开关属性更新：%s", sg_ProductData.m_power_switch? "开启": "关闭");
}

void on_brightness_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                          DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "亮度属性更新：%d", sg_ProductData.m_brightness);
}

void on_color_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                     DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "颜色属性更新：%s", sg_ProductData.m_color_value);
}