//
// Created by Augtons on 2022/9/29.
//

#include "led_strip_impl_ws2812.h"

static const char* TAG = "WS2812";

static uint32_t ws2812_t0h_ticks = 0;
static uint32_t ws2812_t1h_ticks = 0;
static uint32_t ws2812_t0l_ticks = 0;
static uint32_t ws2812_t1l_ticks = 0;


/**
 * @brief Conver RGB data to RMT format.
 *
 * @note For WS2812, R,G,B each contains 256 different choices (i.e. uint8_t)
 *
 * @param[in] src: source data, to converted to RMT format
 * @param[in] dest: place where to store the convert result
 * @param[in] src_size: size of source data
 * @param[in] wanted_num: number of RMT items that want to get
 * @param[out] translated_size: number of source data that got converted
 * @param[out] item_num: number of RMT items which are converted from source data
 */
static void IRAM_ATTR ws2812_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size,
                                         size_t wanted_num, size_t *translated_size, size_t *item_num)
{
    if (src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{ ws2812_t0h_ticks, 1, ws2812_t0l_ticks, 0 }}}; //Logical 0
    const rmt_item32_t bit1 = {{{ ws2812_t1h_ticks, 1, ws2812_t1l_ticks, 0 }}}; //Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pdest = dest;
    while (size < src_size && num < wanted_num) {
        for (int i = 0; i < 8; i++) {
            // MSB first
            if (*psrc & (1 << (7 - i))) {
                pdest->val =  bit1.val;
            } else {
                pdest->val =  bit0.val;
            }
            num++;
            pdest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}

static esp_err_t ws2812_set_color(
    led_strip_t *led_strip,
    uint32_t index,
    uint32_t red,
    uint32_t green,
    uint32_t blue
) {
    ws2812_strip_t *strip = __containerof(led_strip, ws2812_strip_t, parent);

    if (index > strip->led_num) {
        ESP_LOGE(TAG, "The index of led you provided `%d`, is better than strip length `%d`", index, strip->led_num);
        return ESP_ERR_INVALID_SIZE;
    }

    uint32_t head = index * 3;
    // In thr order of GRB
    strip->buffer[head + 0] = green & 0xFF;
    strip->buffer[head + 1] = red & 0xFF;
    strip->buffer[head + 2] = blue & 0xFF;

    return ESP_OK;
}

static esp_err_t ws2812_refresh(led_strip_t *led_strip, TickType_t timeout)
{
    esp_err_t err = ESP_OK;
    ws2812_strip_t *strip = __containerof(led_strip, ws2812_strip_t, parent);
    err = rmt_write_sample(strip->rmt_channel, strip->buffer, 3 * strip->led_num, true);
    if (err != ESP_OK) {
        return err;
    }
    return rmt_wait_tx_done(strip->rmt_channel, timeout);
}

static esp_err_t ws2812_delete(led_strip_t *led_strip) {
    ws2812_strip_t *strip = __containerof(led_strip, ws2812_strip_t, parent);
    rmt_driver_uninstall(strip->rmt_channel);
    free(strip);
    return ESP_OK;
}

static esp_err_t ws2812_clear(led_strip_t *led_strip, TickType_t timeout) {
    ws2812_strip_t *strip = __containerof(led_strip, ws2812_strip_t, parent);
    memset(strip->buffer, 0, 3 * strip->led_num);
    ws2812_refresh(led_strip, timeout);
    return ESP_OK;
}

esp_err_t ws2812_rmt_init(uint8_t channel, gpio_num_t gpio) {
    esp_err_t err = ESP_OK;

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(gpio, channel);
    config.clk_div = 2;

    err = rmt_config(&config);
    if (err != ESP_OK) { return err; }

    err = rmt_driver_install(config.channel, 0, 0);
    if (err != ESP_OK) { return err; }

    return ESP_OK;
}

esp_err_t led_strip_create_ws2812(ws2812_config_t *config, led_strip_t **ret_led_strip) {
    esp_err_t err = ESP_OK;
    struct ws2812_config_t *ws2812Config = config;
    // malloc ws2812 strip type
    ws2812_strip_t *strip = calloc(1, sizeof(ws2812_strip_t) + 3 * ws2812Config->led_num);
    if (strip == NULL) { goto MallocFailed; }

    // call `super` constructor
    strip->parent.set_color = ws2812_set_color;
    strip->parent.delete = ws2812_delete;
    strip->parent.refresh = ws2812_refresh;
    strip->parent.clear = ws2812_clear;

    // call `this` constructor
    strip->rmt_channel = ws2812Config->rmt_channel;
    strip->led_num = ws2812Config->led_num;

    // convert ws2812 timeline to rmt ticks
    uint32_t rmt_counter_clk_hz = 0;
    err = rmt_get_counter_clock(strip->rmt_channel, &rmt_counter_clk_hz);
    if (err != ESP_OK) { goto GetClockFailed; }

    ws2812_t0h_ticks = (uint32_t)((float)rmt_counter_clk_hz * WS2812_T0H_NS / 1e9);
    ws2812_t0l_ticks = (uint32_t)((float)rmt_counter_clk_hz * WS2812_T0L_NS / 1e9);
    ws2812_t1h_ticks = (uint32_t)((float)rmt_counter_clk_hz * WS2812_T1H_NS / 1e9);
    ws2812_t1l_ticks = (uint32_t)((float)rmt_counter_clk_hz * WS2812_T1L_NS / 1e9);

    err = rmt_translator_init(strip->rmt_channel, ws2812_rmt_adapter);
    if (err != ESP_OK) { goto InitFailed; }

    *ret_led_strip = &strip->parent;
    return ESP_OK;

MallocFailed:
    free(strip);
    return ESP_ERR_NO_MEM;
GetClockFailed:
    free(strip);
    return ESP_ERR_INVALID_ARG;
InitFailed:
    free(strip);
    return ESP_FAIL;

}