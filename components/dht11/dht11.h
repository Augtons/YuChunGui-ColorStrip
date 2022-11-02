//
// Created by Augtons on 2022/10/1.
//

#ifndef _DHT11_H
#define _DHT11_H

#ifdef __cplusplus
extern "C" {
#endif

#include "math.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

struct dht11_t {
    gpio_num_t gpio;
    SemaphoreHandle_t lock;
};

typedef struct dht11_t dht11_t;

typedef void *dht11_handle_t;

/**
 * @brief Create DHT11 sensor instance.
 *
 * @param gpio          GPIO to use.
 * @param ret_dht[out]  DHT11 instance created.
 *
 * @return
 *  - ESP_OK succeed
 *  - Others failed
 */
esp_err_t dht11_create(gpio_num_t gpio, dht11_handle_t *ret_dht);

/**
 * @brief Delete DHT11 sensor instance.
 *
 * @param dht11     DHT11 instance to delete.
 *
 * @return
 *  - ESP_OK succeed
 */
esp_err_t dht11_delete(dht11_handle_t dht11);

/**
 * @brief Read temperature and humidity data from DHT11 instance.
 *
 * @param dht11              DHT11 instance
 * @param temperature [out]  The obtained temperature data.
 * @param humidity    [out]  The obtained humidity data.
 *
 * @return
 *  - ESP_OK                Succeed
 *  - ESP_ERR_INVALID_STATE CRC check failed
 *  - ESP_ERR_TIMEOUT       Timeout
 */
esp_err_t dht11_read(dht11_handle_t dht11, float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

#endif //_DHT11_H
