/**
 * ESP32-C3 — 8x RGB LED Complimentary Blink Pattern
 * LEDs : WS2812B-compatible serial LEDs on PIN 10
 * Lib  : FastLED (install via Arduino Library Manager)
 *
 * Blinks in an alternating pattern using complimentary colors,
 * with a smoothly pulsating global brightness.
 */

#include <FastLED.h>

// ── Config ────────────────────────────────────────────────
#define LED_PIN        10       // GPIO connected to DIN of first LED
#define NUM_LEDS       8        // Number of LEDs in the strip
#define LED_TYPE       WS2812B  // Change to SK6812 / WS2811 if needed
#define COLOR_ORDER    GRB      // Most WS2812B strips are GRB
#define MIN_BRIGHTNESS 50       // Lowest pulse brightness
#define MAX_BRIGHTNESS 255      // Highest pulse brightness
#define PULSE_BPM      60       // Speed of the brightness pulse
#define BLINK_INTERVAL 250      // ms between blinks
#define PAIR_DURATION  2000     // ms to show each color pair
// ─────────────────────────────────────────────────────────

CRGB leds[NUM_LEDS];

// ── Palette: Complimentary Color Pairs ────
const uint8_t NUM_PAIRS = 3;
const uint8_t baseHues[NUM_PAIRS] = { 0, 160, 96 }; // Red, Blue, Green
uint8_t currentPair = 0;

// ─────────────────────────────────────────────────────────

void setup() {
  // RMT is used by FastLED on ESP32-C3 — no extra setup needed
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
}

void loop() {
  uint8_t hue1 = baseHues[currentPair];
  uint8_t hue2 = hue1 + 128; // Complementary hue (opposite on color wheel)

  unsigned long startTime = millis();

  // Keep the current complimentary pair active for PAIR_DURATION milliseconds
  while (millis() - startTime < PAIR_DURATION) {
    
    // Calculate which step of the blink we are on (alternates every BLINK_INTERVAL ms)
    uint8_t blinkState = ((millis() - startTime) / BLINK_INTERVAL) % 2;

    // Smoothly pulse the global brightness using FastLED's built-in math
    uint8_t currentBrightness = beatsin8(PULSE_BPM, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    FastLED.setBrightness(currentBrightness);

    for (int i = 0; i < NUM_LEDS; i++) {
      if ((i % 2) == blinkState) {
        leds[i] = CHSV(hue1, 255, 255);
      } else {
        leds[i] = CHSV(hue2, 255, 255);
      }
    }
    
    FastLED.show();
    
    // A tiny delay to allow the ESP32 to do background tasks and keep framerate smooth
    delay(10);
  }

  // Move to the next complimentary color pair
  currentPair = (currentPair + 1) % NUM_PAIRS;
}
