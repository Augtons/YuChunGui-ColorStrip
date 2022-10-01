#include <stdio.h>

#include "led_strip.h"
#include "led_strip_impl_ws2812.h"

#include "esp_task_wdt.h"

#include "dht11.h"

static const char* TAG = "MAIN";

void app_main(void)
{
    ws2812_rmt_init(RMT_CHANNEL_0, GPIO_NUM_7);

    led_strip_t *strip = NULL;

    ws2812_config_t strip_config = {
        .rmt_channel = RMT_CHANNEL_0,
        .led_num = 60,
        .gpio = GPIO_NUM_7
    };

    uint32_t colors[12][3] = {0};

    led_strip_create_ws2812(&strip_config, &strip);

    bool direction = 1;
    uint32_t light = 10;

    dht11_handle dht11 = NULL;
    dht11_create(GPIO_NUM_6, &dht11);

    float temp, humi;

    while (1) {

//        for (int i = 0; i < 12; i++) {
//            hsv2rgb(i * 30, 100, light, &colors[i][0], &colors[i][1], &colors[i][2]);
//        }
//
//        if (light >= 20) {
//            direction = 0;
//        } else if (light <= 5) {
//            direction = 1;
//        }
//
//        direction ? light++ : light--;
//
//        for(int i = 0; i < 60; i++) {
//            led_strip_set_color(strip, i, colors[i%12][0], colors[i%12][1], colors[i%12][2]);
//        }
//        led_strip_refresh(strip, 10);

        vTaskDelay(pdMS_TO_TICKS(1000));

        temp = 0;
        humi = 0;
        esp_err_t err = dht11_read(dht11, &temp, &humi);

        ESP_LOGI("main", "temp = %f, humi = %f, err: %s", temp, humi, esp_err_to_name(err));

    }

}
