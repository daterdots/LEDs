#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS    60
#define DATA_PIN    3
#define COLOR_ORDER GRB
#define CHIPSET     NEOPIXEL

CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(255);
}
void loop() { 
  static uint8_t hue = 0;
  FastLED.showColor(CHSV(hue++, 255, 255)); 
  delay(100);
}
