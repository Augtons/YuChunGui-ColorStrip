#ifndef _LED_STRIP_H_
#define _LED_STRIP_H_

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

typedef void * led_strip_config_t;

typedef struct led_strip_func_t led_strip_t;

typedef void * led_strip_dev_t;

void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

struct led_strip_func_t {
    esp_err_t (*delete)(led_strip_t *led_strip);

    esp_err_t (*set_color)(led_strip_t *led_strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

    esp_err_t (*refresh)(led_strip_t *led_strip, TickType_t timeout);

    esp_err_t (*clear)(led_strip_t *led_strip, TickType_t timeout);
};

esp_err_t led_strip_refresh(led_strip_t *led_strip, TickType_t timeout);

esp_err_t led_strip_delete(led_strip_t *led_strip);

esp_err_t led_strip_set_color(led_strip_t *led_strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

esp_err_t led_strip_clear(led_strip_t *led_strip, TickType_t timeout);

#endif