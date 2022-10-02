//
// Created by Augtons on 2022/10/1.
//

#include "dht11.h"

static const char *TAG = "DHT11";

static esp_err_t wait_for_gpio_level_change(const gpio_num_t gpio, const uint8_t current_level, const uint32_t timeout) {
    uint64_t timeout_time = esp_timer_get_time() + timeout;

    while (gpio_get_level(gpio) == current_level) {
        if (esp_timer_get_time() > timeout_time) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

esp_err_t dht11_create(gpio_num_t gpio, dht11_handle_t *ret_dht) {
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
    esp_err_t err = gpio_config(&gpiocfg);
    if (err != ESP_OK) { return  err; }

    *ret_dht = dht11;

    return ESP_OK;
}

esp_err_t dht11_delete(dht11_handle_t dht11) {
    dht11_t *dht = dht11;
    free(dht);
    return ESP_OK;
}

esp_err_t dht11_read(dht11_handle_t dht11, float *temperature, float *humidity) {
    dht11_t *dht = dht11;
    esp_err_t err = ESP_OK;

    // 1. make a start signal.
    gpio_set_level(dht->gpio, 1);
    gpio_set_level(dht->gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(19)); //至少拉低18ms
    gpio_set_level(dht->gpio, 1);

    // 2. wait for ack signal.
    esp_rom_delay_us(70);
    if (gpio_get_level(dht->gpio) == 1) {
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
            ESP_RETURN_ON_ERROR(err, TAG, "Timeout when reading data.");

            uint64_t time = esp_timer_get_time() + 50;
            while (gpio_get_level(dht->gpio)) {
                if (esp_timer_get_time() > time) {
                    buf |= 1 << (7 - i);
                    err = wait_for_gpio_level_change(dht->gpio, 1, 100);
                    ESP_RETURN_ON_ERROR(err, TAG, "Timeout when reading data.");
                    break;
                }
            }
        }
        byte[b] = buf;
    }

    /*
     * byte[0] is humidity integer
     * byte[1] is humidity decimal
     * byte[2] is temperature integer
     * byte[3] is temperature decimal
     * byte[4] is crc
     */

    float humi_decimal_float = byte[1];
    float temp_decimal_float = byte[3];

    while (humi_decimal_float /= 10, humi_decimal_float >= 1);
    while (temp_decimal_float /= 10, temp_decimal_float >= 1);

    if ((uint8_t)(byte[0] + byte[1] + byte[2] + byte[3]) != byte[4]) {
        ESP_LOGW(TAG, "CRC check failed. Data: %X-%X-%X-%X => %X", byte[0], byte[1], byte[2], byte[3], byte[4]);
        return ESP_ERR_INVALID_STATE;
    }

    *temperature = (float)byte[2] + roundf(temp_decimal_float * 10) / 10;
    *humidity = (float)byte[0] + roundf(humi_decimal_float * 10 / 10);

    return ESP_OK;
}