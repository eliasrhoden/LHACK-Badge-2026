/**
 * ESP32-C3 — 8x RGB LED Color Fade
 * LEDs : WS2812B-compatible serial LEDs on PIN 10
 * Lib  : FastLED (install via Arduino Library Manager)
 *
 * Fades smoothly through Blue → Purple → Green → Blue …
 */

#include <FastLED.h>

// ── Config ────────────────────────────────────────────────
#define LED_PIN     10          // GPIO connected to DIN of first LED
#define NUM_LEDS    8           // Number of LEDs in the strip
#define LED_TYPE    WS2812B     // Change to SK6812 / WS2811 if needed
#define COLOR_ORDER GRB         // Most WS2812B strips are GRB
#define BRIGHTNESS  180         // 0–255  (lower = cooler & less current)
#define FADE_SPEED  5           // ms between each fade step (lower = faster)
// ─────────────────────────────────────────────────────────

CRGB leds[NUM_LEDS];

// ── Palette: Blue → Purple → Green (and back to Blue) ────
//    Each entry is a CHSV hue value (0–255 maps to 0°–360°)
//      Blue   ≈ hue 160
//      Purple ≈ hue 192
//      Green  ≈ hue 96
const uint8_t NUM_COLORS = 3;
const uint8_t targetHues[NUM_COLORS] = { 160, 192, 96 };

uint8_t currentHue  = 160;
uint8_t currentColor = 0;       // index into targetHues[]

// ─────────────────────────────────────────────────────────

void setup() {
  // RMT is used by FastLED on ESP32-C3 — no extra setup needed
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // Start with all LEDs blue
  fill_solid(leds, NUM_LEDS, CHSV(currentHue, 255, 255));
  FastLED.show();
}

void loop() {
  uint8_t nextColor = (currentColor + 1) % NUM_COLORS;
  uint8_t targetHue = targetHues[nextColor];

  // Fade hue from currentHue → targetHue one step at a time.
  // We move in the shortest direction around the hue wheel.
  while (currentHue != targetHue) {
    // Signed 8-bit arithmetic gives us the shortest-path direction.
    int8_t delta = (int8_t)(targetHue - currentHue);
    currentHue += (delta > 0) ? 1 : -1;

    fill_solid(leds, NUM_LEDS, CHSV(currentHue, 255, 255));
    FastLED.show();
    delay(FADE_SPEED);
  }

  // Pause briefly at the target color before moving on
  delay(400);
  currentColor = nextColor;
}
