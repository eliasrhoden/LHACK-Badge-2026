/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 10 //CONFIG_BLINK_GPIO

    

#ifdef CONFIG_BLINK_LED_STRIP

static led_strip_handle_t led_strip;


int left[6] = {
    0, 1, 2, 3, 2, 1
};

int right[6] = {
    7, 6, 5, 4, 5, 6
};

int data[8] = {0,1,2,3,4,5,6,7};

typedef struct {
    int r;
    int g;
    int b;
} RGB;

RGB colors[] = {
    {255, 0, 0},
    {255, 64, 0},
    {255, 128, 0},
    {255, 192, 0},
    {255, 255, 0},
    {192, 255, 0},
    {128, 255, 0},
    {64, 255, 0},
    {0, 255, 0},
    {0, 255, 64},
    {0, 255, 128},
    {0, 255, 192},
    {0, 255, 255},
    {0, 192, 255},
    {0, 128, 255},
    {0, 64, 255},
    {0, 0, 255},
    {64, 0, 255},
    {128, 0, 255},
    {192, 0, 255},
    {255, 0, 255},
    {255, 0, 192},
    {255, 0, 128},
    {255, 0, 64},
};

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    static int pixel = 0;

    int l_v = left[pixel % 6];
    int h_v = right[pixel % 6];

    int subtract[6];
    int j = 0;
    for (int i = 0; i < 8; i++) {
        if (data[i] != l_v && data[i] != h_v) {
            subtract[j++] = data[i];
        }
    }

    // COLOR
    int l_r = colors[pixel % 24].r;
    int l_g = colors[pixel % 24].g;
    int l_b = colors[pixel % 24].b;

    // LIGHT
    led_strip_set_pixel(led_strip, l_v, l_r, l_g, l_b);
    led_strip_set_pixel(led_strip, h_v, 255 - l_r, 255 - l_g, 255 - l_b);
    
    // DARKNESS
    for (int i = 0; i < 6; i++) {
        led_strip_set_pixel(led_strip, subtract[i], 0, 0, 0);
    }

    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);
    pixel++;

    /* Set all LED off to clear all pixels */
    // led_strip_clear(led_strip);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 8, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

#elif CONFIG_BLINK_LED_GPIO

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#else
#error "unsupported LED type"
#endif

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();

    while (1) {
        //ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        vTaskDelay(400 / portTICK_PERIOD_MS);
    }
    }