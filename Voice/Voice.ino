/**
 * ESP32-C3 — 8x RGB LED PC Voice Visualizer
 * LEDs : WS2812B-compatible serial LEDs on PIN 10
 * Data : Receives volume data over USB Serial from a PC Python script
 * Lib  : FastLED (install via Arduino Library Manager)
 *
 * Acts as a volume meter/visualizer for PC audio input.
 */

#include <FastLED.h>

// ── Config ────────────────────────────────────────────────
#define LED_PIN        10       // GPIO connected to DIN of first LED
#define NUM_LEDS       8        // Number of LEDs in the strip
#define LED_TYPE       WS2812B  // Change to SK6812 / WS2811 if needed
#define COLOR_ORDER    GRB      // Most WS2812B strips are GRB
#define BRIGHTNESS     100      // 0-255

// Serial Data Config
#define NOISE_FLOOR    5        // Adjust based on the Python script's output noise floor
#define MAX_VOLUME     50       // Adjust based on how loud you expect the max volume to be from the script
#define MIN_FREQ       100      // Minimum frequency to light up first LED
#define MAX_FREQ       300      // Frequency to light up all LEDs
// ─────────────────────────────────────────────────────────

CRGB leds[NUM_LEDS];
int currentVolume = 0;
int currentFreq = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10); // Very important so parseInt doesn't block the animation loop
  
  // RMT is used by FastLED on ESP32-C3 — no extra setup needed
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  // Read the serial data from the Python script if available
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    int commaIndex = data.indexOf(',');
    if (commaIndex > 0) {
      currentVolume = data.substring(0, commaIndex).toInt();
      currentFreq = data.substring(commaIndex + 1).toInt();
    } else {
      currentVolume = data.toInt();
    }
  }
  
  int micValue = currentVolume;

  // Remove background noise
  if (micValue < NOISE_FLOOR) {
    micValue = 0;
  }

  // Tone and Intensity -> Number of LEDs
  int ledsToLight = 0;
  if (micValue >= NOISE_FLOOR) {
    // Calculate how many LEDs pitch wants to light up
    int freqLeds = map(currentFreq, MIN_FREQ, MAX_FREQ, 0, NUM_LEDS);
    // Calculate how many LEDs volume wants to light up
    int volLeds = map(micValue, NOISE_FLOOR, MAX_VOLUME, 0, NUM_LEDS);
    
    // Add both together so both pitch and intensity push the lights higher
    ledsToLight = freqLeds + volLeds;
    ledsToLight = constrain(ledsToLight, 1, NUM_LEDS);
  }

  // Intensity (volume) -> Color and Brightness
  // Hue: Low volume -> Blue/Purple (160), High volume -> Red (0)
  int mappedHue = map(micValue, NOISE_FLOOR, MAX_VOLUME, 160, 0);
  mappedHue = constrain(mappedHue, 0, 255);
  
  // Brightness: Low volume -> Dim, High volume -> Bright
  int mappedVal = map(micValue, NOISE_FLOOR, MAX_VOLUME, 50, 255);
  mappedVal = constrain(mappedVal, 0, 255);

  // Fade all LEDs slightly to create a smooth "falling" effect
  fadeToBlackBy(leds, NUM_LEDS, 60);

  // Light up the LEDs based on the calculated values
  for (int i = 0; i < ledsToLight; i++) {
    leds[i] = CHSV(mappedHue, 255, mappedVal);
  }

  FastLED.show();
  
  // Small delay for stability and to control the fall speed of the meter
  delay(15);
}
