# DHT11 component for esp32

Object-oriented design is used. Support for ESP-IDF 4.4.x now.

-----

## Usage

```c
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

static const char *TAG = "MAIN"; 

// 1. Declare a DHT11 sensor object.
dht11_handle_t dht11 = NULL;

void app_main() {
    // 2. Create a DHT11 instance.
    dht11_create(GPIO_NUM_6, &dht11);
    
    float temp = 0;
    float humi = 0;
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // 3. Read temperature and humidity.
        esp_err_t err = dht11_read(dht11, &temp, &humi);
        
        // 4. Check error
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
        }
        
        // 5. Print result.
        ESP_LOGI(TAG, "temp = %.2f, humi = %.2f", temp, humi);
    }
}

```
