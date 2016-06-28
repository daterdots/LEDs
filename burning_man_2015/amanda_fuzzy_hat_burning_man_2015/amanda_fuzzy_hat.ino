#include "FastLED.h"
//#include <avr/power.h>

// How many leds in your strip?
#define NUM_LEDS 120

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 3
// #define CLOCK_PIN 13 //no clock pin necessary on NEOPIXELs

// Define the array of leds
CRGB leds[NUM_LEDS];
//CRGB storage[NUM_LEDS / 2];
//CRGB storage2[NUM_LEDS / 2];
uint8_t count = 0; //count is an unsigned 8 bit integer limited to values between 0 and 255
uint8_t walker = 0;
#define low 20
#define high  150
//uint8_t center = 29;
#define delay_time 20

void setup() {
  //clock_prescale_set(clock_div_2);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  //Serial.begin(9600);
}

void loop() {
  // First, set the colors of all the LEDs on the strip
  for (int j = low; j < high; j++) {
    for (int i = 0; i < NUM_LEDS / 2; i++) {
      // Set the i'th led a color defined by a multiple of i plus 'count'
      walker = i + count;
      if (walker % 2 == 1) {
        leds[i] = CHSV(count + i * 8, 190, j);
        //Serial.println(j);
      }
      else {
        leds[i] = CRGB::Black;
      }
    }

    //for (int i = 0; i < NUM_LEDS / 2; i++) {
    //  storage2[i] = leds[NUM_LEDS / 2 - i - 1];
    //}

    for (int i = 0; i < NUM_LEDS / 2; i++) {
      leds[NUM_LEDS - i] = leds[i];
    }
    count++; //index count up
    // Show the leds
    FastLED.show();
    // Wait a little bit before we loop around and do it again
    delay(delay_time);
  }

  ////////////
  for (int j = high; j > low; j--) {
    for (int i = 0; i < NUM_LEDS / 2; i++) {
      // Set the i'th led a color defined by a multiple of i plus 'count'
      walker = i + count;
      if (walker % 2 == 1) {
        leds[i] = CHSV(count + i * 8, 190, j);
        //Serial.println(j);
      }
      else {
        leds[i] = CRGB::Black;
      }
    }

    //for (int i = 0; i < NUM_LEDS / 2; i++) {
    //  storage2[i] = leds[NUM_LEDS / 2 - i - 1];
    //}

    for (int i = 0; i < NUM_LEDS / 2; i++) {
      leds[NUM_LEDS - i] = leds[i];
    }
    count++; //index count up
    // Show the leds
    FastLED.show();
    // Wait a little bit before we loop around and do it again
    delay(delay_time);
  }
}
