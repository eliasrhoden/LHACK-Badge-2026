#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "led_marquee";

// If you haven't set this in menuconfig, you can hardcode it:
// #define BLINK_GPIO 10 
#define BLINK_GPIO CONFIG_BLINK_GPIO
#define NUM_LEDS   8

static led_strip_handle_t led_strip;
static uint8_t g_current_led = 0; // Tracks which LED is currently on

static void blink_led(void)
{
    /* 1. Clear the entire strip (turn everything off) */
    led_strip_clear(led_strip);

    /* 2. Set only the "current" LED to a color (Red in this example) */
    /* Parameters: (handle, index, red, green, blue) */
    led_strip_set_pixel(led_strip, g_current_led, 0, 0, 50);

    /* 3. Push the data to the strip */
    led_strip_refresh(led_strip);

    ESP_LOGI(TAG, "LED %d is shining!", g_current_led);

    /* 4. Increment the counter and wrap around back to 0 using modulo */
    g_current_led = (g_current_led + 1) % NUM_LEDS;
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Configuring LED strip on GPIO %d", BLINK_GPIO);
    
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = NUM_LEDS, 
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // Standard for most 5050 RGBs
        .led_model = LED_MODEL_WS2812,           // Your TZ-5050 uses this protocol
    };

    // RMT is the standard backend for ESP32-C3 addressable LEDs
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);
}

void app_main(void)
{
    configure_led();

    while (1) {
        blink_led();
        // Adjust the delay to change the speed of the "walk"
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}