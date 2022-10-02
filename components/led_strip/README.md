# LED Strip component for esp32 SoC

Object-oriented design is used. Support for ESP-IDF 4.4.x now.

## LED strips Supported

- WS2812
- will comming soon...

## Usage

```c
#include "led_strip.h"
#include "led_strip_impl_ws2812.h"

// 1. Declare LED strip object
led_strip_t *strip = NULL;

void app_main()
{
    // 2. Initialize the RMT device.
    //    That's a sample function to initialize RMT device for ws2812 strip in this component.
    //    !!! You can make your own way !!!
    ws2812_rmt_init(RMT_CHANNEL_0, GPIO_NUM_7);
    
    // 3. Make a structure to pass configuration.
    ws2812_config_t strip_config = {
        .rmt_channel = RMT_CHANNEL_0, /* RMT channel for the strip */
        .gpio = GPIO_NUM_7            /* GPIO which connected to the strip. */
        .led_num = 60,                /* Count of LEDs. */
    };

    // 4. Create strip object.
    //    Take WS2812, for example:
    led_strip_create_ws2812(&strip_config, &strip);
    
    // 5. Set in RGB color space and display.
    //    Take 胭脂(R: 157, G: 41, B: 51), for example: (index 0)
    led_strip_set_color(strip, 0, 62, 237, 231);
    led_strip_refresh(strip, 10); // 10 is timeout(freertos tick)
    
    // 6. Set in HSV color space and display.
    //    Take 靛青(H: 200, S: 87, V: 69), for example: (index 1)
    uint32_t r = 0, g = 0, b = 0;
    hsv2rgb(200, 87, 69, &r, %g, &b);
    led_strip_set_color(strip, 1, r, g, b);
    led_strip_refresh(strip, 10); // 10 is timeout(freertos tick)
    
    // 7. Turn off all LEDs.
    led_strip_clear(strip, 10); // 10 is timeout(freertos tick)
    
    // 8. Delete the led strip object.
    led_strip_delete(strip);

}
```