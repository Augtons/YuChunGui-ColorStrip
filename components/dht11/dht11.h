//
// Created by Augtons on 2022/10/1.
//

#ifndef _DHT11_H
#define _DHT11_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct dht11_t {
    gpio_num_t gpio;
};

typedef struct dht11_t dht11_t;

typedef void *dht11_handle;

esp_err_t dht11_create(gpio_num_t gpio, dht11_handle *ret_dht);

esp_err_t dht11_delete(dht11_handle dht11);

esp_err_t dht11_read(dht11_handle dht11, float *temperature, float *humidity);

#endif //_DHT11_H
