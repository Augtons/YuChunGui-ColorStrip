#ifndef _LED_STRIP_H_
#define _LED_STRIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

typedef void *led_strip_config_t;

typedef struct led_strip_func_t led_strip_t;

typedef void *led_strip_dev_t;

void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

struct led_strip_func_t {

    esp_err_t (*delete)(led_strip_t *led_strip);

    esp_err_t (*set_color)(led_strip_t *led_strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

    esp_err_t (*refresh)(led_strip_t *led_strip, TickType_t timeout);

    esp_err_t (*clear)(led_strip_t *led_strip, TickType_t timeout);
};

/**
 * @brief Refresh and display the colors was set.
 *
 * @param led_strip LED strip object.
 * @param timeout   Timeout in freertos tick.
 *
 * @return
 *   - ESP_OK succeed
 *   - Other  failed
 */
esp_err_t led_strip_refresh(led_strip_t *led_strip, TickType_t timeout);

/**
 * @brief Delete strip object. And free the memory used.
 *
 * @param led_strip Strip object to delete.
 * @return
 *   - ESP_OK succeed
 *   - Other  failed
 */
esp_err_t led_strip_delete(led_strip_t *led_strip);

/**
 * @brief Sets the color of the LED at an index.
 *
 * @param led_strip     led_strip object
 * @param index         Index to set
 * @param red           Red of RGB color space. Between 0 to 255
 * @param green         Green of RGB color space. Between 0 to 255
 * @param blue          Blue of RGB color space. Between 0 to 255
 *
 * @return
 *   - ESP_OK succeed
 *   - Other  failed
 */
esp_err_t led_strip_set_color(led_strip_t *led_strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

/**
 * @brief Turn off all LEDs and clear color was set right away.
 *
 * @param led_strip LED strip object.
 * @param timeout   Timeout in freertos tick.
 *
 * @return
 *   - ESP_OK succeed
 *   - Other  failed
 */
esp_err_t led_strip_clear(led_strip_t *led_strip, TickType_t timeout);

#ifdef __cplusplus
}
#endif

#endif