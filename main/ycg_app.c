//
// Created by augtons on 22-11-8.
//
#include "math.h"

#include "system.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "tencent_cloud_app.h"
#include "dht11.h"
#include "led_strip.h"
#include "led_strip_impl_ws2812.h"

static const char *TAG = "YCG_APP";

#define dht11           ycg_system.devices.dht11
#define ledStrip        ycg_system.devices.ledStrip

static TaskHandle_t led_strip_task;

_Noreturn void ycg_app(void *arg) {
    esp_err_t err = ESP_OK;

    xTaskCreate(led_strip_display, "led_strip_display", 4096, NULL, 3, &led_strip_task);

    float temp = 0, humi = 0;

    loop {
        dht11_read(dht11, &temp, &humi);
        synchronized(ycg_data_lock, {
            sg_ProductData.m_temp = temp;
            sg_ProductData.m_humi = (int) humi;
        })
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void on_blufi_custom_data_receive(uint8_t *data, uint32_t len) {
    if (memcmp(data, "reboot", len) == 0) {
        ESP_LOGI(TAG, "RESTART");
        esp_restart();
    }
}

static inline void led_strip_display_single(float r, float g, float b, float brightness) {
    for (int i = 0; i < LED_NUMS; i++) {
        led_strip_set_color(ledStrip, i,
                            (uint32_t)(r * brightness / 100.0F),
                            (uint32_t)(g * brightness / 100.0F),
                            (uint32_t)(b * brightness / 100.0F)
        );
    }
    led_strip_refresh(ledStrip, pdMS_TO_TICKS(50));
}

static inline void led_strip_display_flow(float r, float g, float b, float brightness) {
    static int step = 0; // [0, 19]
    for (int i = 0; i < LED_NUMS; i++) {
        float step_brightness;
        int temp = (i + step) % 20;
        if (temp <= 15) {
            step_brightness = brightness * (15.0F - temp) / 15.0F;
        } else {
            step_brightness = 0;
        }
        led_strip_set_color(ledStrip, i,
                            (uint32_t)(r * step_brightness / 100.0F),
                            (uint32_t)(g * step_brightness / 100.0F),
                            (uint32_t)(b * step_brightness / 100.0F)
        );
    }
    led_strip_refresh(ledStrip, pdMS_TO_TICKS(50));
    step ++;
    if (step >= 20) step = 0;
}

static inline void led_strip_display_rainbow(float brightness) {
    static int step = 0; //[0-35]
    uint32_t r,g,b;
    for (int i = 0; i < LED_NUMS; i++) {
        hsv2rgb(((i + step) % 36) * 10, 100, (uint32_t)brightness, &r, &g, &b);
        led_strip_set_color(ledStrip, i, r, g, b);
    }
    led_strip_refresh(ledStrip, pdMS_TO_TICKS(50));
    step ++;
    if (step >= 36) step = 0;
}

_Noreturn void led_strip_display(void *arg) {
    enum DisplayMode mode = RAINBOW;
    uint8_t red = 0, green = 0, blue = 0;
    float brightness = 0;
    bool updated = true;

    loop {
        if (ulTaskNotifyTake(true, 0)) {
            updated = true;
            synchronized(ycg_data_lock, {
                mode = sg_ProductData.m_color_mode;
                red = sg_ProductData.m_color_red;
                green = sg_ProductData.m_color_green;
                blue = sg_ProductData.m_color_blue;
                brightness = sg_ProductData.m_brightness / 16.0F;
            })
        }
        switch (mode) {
            case SINGLE: {
                if (updated) {
                    led_strip_display_single(red, green, blue, brightness);
                }
                break;
            }
            case FLOW: {
                led_strip_display_flow(red, green, blue, brightness);
                break;
            }
            case RAINBOW: {
                led_strip_display_rainbow(brightness);
                break;
            }
            default: {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        updated = false;
    }
}

void on_power_switch_state_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                                  DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "开关属性更新：%s", sg_ProductData.m_power_switch? "开启": "关闭");
    if (led_strip_task == NULL) {
        return;
    }
    if (sg_ProductData.m_power_switch) {
        vTaskResume(led_strip_task);
    } else {
        vTaskSuspend(led_strip_task);
        led_strip_clear(ledStrip, pdMS_TO_TICKS(100));
    }
    update_state();
}

void on_brightness_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                          DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "亮度属性更新：%d", sg_ProductData.m_brightness);
    xTaskNotifyGive(led_strip_task);
    update_state();
}

void on_color_mode_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength,
                     DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "颜色模式更新：%d", sg_ProductData.m_color_mode);
    xTaskNotifyGive(led_strip_task);
    update_state();
}

void on_color_red_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "颜色Red值更新：%d", sg_ProductData.m_color_red);
    xTaskNotifyGive(led_strip_task);
    update_state();
}

void on_color_green_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "颜色Green值更新：%d", sg_ProductData.m_color_green);
    xTaskNotifyGive(led_strip_task);
    update_state();
}

void on_color_blue_change(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty) {
    ESP_LOGI(TAG, "颜色Blue值更新：%d", sg_ProductData.m_color_blue);
    xTaskNotifyGive(led_strip_task);
    update_state();
}
