#include <Adafruit_NeoPixel.h>

// Pin to use to send signals to WS2812B
#define LED_PIN 10

// Number of WS2812B LEDs attached to the Arduino
#define LED_COUNT 8

// Setting up the NeoPixel library
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();           // Initialize NeoPixel object
  strip.setBrightness(10); // Set BRIGHTNESS to about 4% (max = 255)
  strip.clear();
}

void loop() {
  // strip.clear(); // Set all pixel colors to 'off'
  // strip.show();   // Send the updated pixel colors to the hardware.
  // delay(500); // Pause before next pass through loop
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<LED_COUNT; i++) {
    if (i == 0) {
      strip.setPixelColor(LED_COUNT - 1, 255, 0, 0);
      strip.setPixelColor(LED_COUNT - 2, 0, 0, 0);
    } else if (i == 1)  {
      strip.setPixelColor(i - 1, 255, 0, 0);
      strip.setPixelColor(LED_COUNT - 1, 0, 0, 0);
    } else {
      strip.setPixelColor(i - 1, 255, 0, 0);
      strip.setPixelColor(i - 2, 0, 0, 0);
    }
    // Set the i-th LED to pure green:
    strip.setPixelColor(i, 0, 255, 0);
  
    strip.show();   // Send the updated pixel colors to the hardware.
  
    delay(500); // Pause before next pass through loop
  }
}