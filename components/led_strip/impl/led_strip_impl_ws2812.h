//
// Created by Augtons on 2022/9/29.
//

#ifndef _LED_STRIP_IMPL_WS2812_H
#define _LED_STRIP_IMPL_WS2812_H

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
    rmt_channel_t   rmt_channel; /* RMT channel for the strip */
    gpio_num_t      gpio;        /* GPIO which connected to the strip. */
    uint32_t        led_num;     /* Count of LEDs. Determines how much memory to be allocated. */
};

typedef struct ws2812_strip_t ws2812_strip_t;
typedef struct ws2812_config_t ws2812_config_t;

/**
 * @brief A sample function to initialize RMT device for ws2812 strip.
 *
 * @param channel       [in] RMT channel to initialize.
 * @param gpio          [out] GPIO to use.
 *
 * @return ESP_OK success
 *
 * @author Augtons
 */
esp_err_t ws2812_rmt_init(uint8_t channel, gpio_num_t gpio);

/**
 * @briefCreate Create a ws2812 led_strip instance.
 *
 * @param config        [in]  configuration of it.
 * @param ret_led_strip [out] led_strip object which were created.
 *
 * @return
 *   - ESP_OK succeed
 *   - Other  failed
 *
 * @Author Augtons
 */
esp_err_t led_strip_create_ws2812(ws2812_config_t *config, led_strip_t **ret_led_strip);

#endif //_LED_STRIP_IMPL_WS2812_H
