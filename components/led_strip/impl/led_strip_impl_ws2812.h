//
// Created by Augtons on 2022/9/29.
//

#ifndef YUCHUNGUI_COLORSTRIP2_LED_STRIP_IMPL_WS2812_H
#define YUCHUNGUI_COLORSTRIP2_LED_STRIP_IMPL_WS2812_H

#include "string.h"

#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "interface/led_strip.h"

#define WS2812_T0H_NS (350)
#define WS2812_T0L_NS (1000)
#define WS2812_T1H_NS (1000)
#define WS2812_T1L_NS (350)
#define WS2812_RESET_US (280)

struct ws2812_strip_t {
    led_strip_t         parent;
    rmt_channel_t       rmt_channel;
    uint32_t            led_num;
    uint8_t             buffer[0];
};

struct ws2812_config_t {
    rmt_channel_t   rmt_channel;
    gpio_num_t      gpio;
    uint32_t        led_num;
};

typedef struct ws2812_strip_t ws2812_strip_t;
typedef struct ws2812_config_t ws2812_config_t;

esp_err_t ws2812_rmt_init(uint8_t channel, gpio_num_t gpio);

esp_err_t led_strip_create_ws2812(ws2812_config_t *config, led_strip_t **ret_led_strip);

#endif //YUCHUNGUI_COLORSTRIP2_LED_STRIP_IMPL_WS2812_H
