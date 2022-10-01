//
// Created by Augtons on 2022/10/1.
//

#include "dht11.h"

static const char *TAG = "DHT11";

static esp_err_t wait_for_gpio_level_change(const gpio_num_t gpio, const uint8_t current_level, const uint32_t timeout) {
    if (timeout < 0) { return ESP_ERR_INVALID_ARG; }
    uint64_t timeout_time = esp_timer_get_time() + timeout;

    while (gpio_get_level(gpio) == current_level) {
        if (esp_timer_get_time() > timeout_time) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

esp_err_t dht11_create(gpio_num_t gpio, dht11_handle *ret_dht) {
    dht11_t *dht11 = calloc(1, sizeof(dht11_t));
    dht11->gpio = gpio;

    gpio_pad_select_gpio(gpio);
    gpio_config_t gpiocfg = {
        .pin_bit_mask = BIT64(dht11->gpio),
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpiocfg);

    *ret_dht = dht11;

    return ESP_OK;
}

esp_err_t dht11_delete(dht11_handle dht11) {
    dht11_t *dht = dht11;
    free(dht);
    return ESP_OK;
}

esp_err_t dht11_read(dht11_handle dht11, float *temperature, float *humidity) {
    dht11_t *dht = dht11;
    esp_err_t err = ESP_OK;

    uint64_t mydata = 0;

    // 1. make a start signal.
    gpio_set_level(dht->gpio, 1);
    gpio_set_level(dht->gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(19)); //至少拉低18ms
    gpio_set_level(dht->gpio, 1);

    // 2. wait for ack signal.
    esp_rom_delay_us(70); // 等待检测应答信号
    if (gpio_get_level(dht->gpio) == 1) {
        // 未应答
        return ESP_ERR_TIMEOUT;
    }

    // 3. wait for ack signal low level finish.
    err = wait_for_gpio_level_change(dht->gpio, 0, 100);
    ESP_RETURN_ON_ERROR(err, TAG, "Timeout when waiting for ack signal low level finish.");

    // 4. wait for ack signal high level finish.
    err = wait_for_gpio_level_change(dht->gpio, 1, 100);
    ESP_RETURN_ON_ERROR(err, TAG, "Timeout when waiting for ack signal high level finish.");

    uint8_t byte[5] = {0};

    for(int b = 0; b < 5; b++) {
        uint8_t buf = 0;
        for (int i = 0; i < 8; i++) {
            err = wait_for_gpio_level_change(dht->gpio, 0, 100);
            ESP_RETURN_ON_ERROR(err, TAG, "Timeout");

            uint64_t time = esp_timer_get_time() + 50;
            while (gpio_get_level(dht->gpio)) {
                if (esp_timer_get_time() > time) {
                    buf |= 1 << (7 - i);
                    err = wait_for_gpio_level_change(dht->gpio, 1, 100);
                    ESP_RETURN_ON_ERROR(err, TAG, "Timeout");
                    break;
                }
            }
        }
        byte[b] = buf;
    }

    uint8_t humi_integer = byte[0];
    uint8_t humi_decimal = byte[1];
    uint8_t temp_integer = byte[2];
    uint8_t temp_decimal = byte[3];
    uint8_t jiaoyan = byte[4];

    if ((uint8_t)(humi_integer + humi_decimal + temp_integer + temp_decimal) != jiaoyan) {
        return ESP_ERR_INVALID_STATE;
    }

    *temperature = (float)temp_integer;
    *humidity = (float)humi_integer;

    return ESP_OK;
}