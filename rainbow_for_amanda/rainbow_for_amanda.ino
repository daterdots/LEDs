#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 60

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 11
// #define CLOCK_PIN 13 //no clock pin necessary on NEOPIXELs

// Define the array of leds
CRGB leds[NUM_LEDS];
uint8_t count = 0; //count is an unsigned 8 bit integer limited to values between 0 and 255
uint8_t walker = 0;
uint8_t low = 50;
uint8_t high = 150;
uint8_t center = 29;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
  // First, set the colors of all the LEDs on the strip
  for (int j = low; j < high; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      // Set the i'th led a color defined by a multiple of i plus 'count'
      walker = i + count;
      if (walker % 5 == 1) {
        leds[i] = CHSV(count + i, 190, j);
      }
      else {
        leds[i] = CRGB::Black;
      }
    }
    count++; //index count up
    // Show the leds
    FastLED.show();
    // Wait a little bit before we loop around and do it again
    delay(35);
  }
  for (int j = high; j >= low; j--) {
    for (int i = 0; i < NUM_LEDS; i++) {
      // Set the i'th led a color defined by a multiple of i plus 'count'
      walker = i + count;
      if (walker % 5 == 1) {
        leds[i] = CHSV(count + i, 190, j);
      }
      else {
        leds[i] = CRGB::Black;
      }
    }
    count++; //index count up
    // Show the leds
    FastLED.show();
    // Wait a little bit before we loop around and do it again
    delay(35);
  }
}



