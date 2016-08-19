#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS 30
#define DATA_PIN 3
CRGB leds[NUM_LEDS];
int snakeLength = 20;
#define color Blue;

// This function sets up the ledsand tells the controller about them
void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<NEOPIXEL, DATA_PIN, RGB>(leds, NUM_LEDS);
  LEDS.setBrightness(10);
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() 
{
  // Move a single white led 
  for(int snakeHead = 0; snakeHead < (NUM_LEDS + snakeLength); snakeHead = snakeHead + 1) 
  {
    // Turn our current led on to white, then show the leds

    for(int i = 0; i < snakeLength; i++)
    {
      if (snakeHead - i >= 0 && snakeHead < NUM_LEDS -i)
      {
        leds[snakeHead - i] = CRGB::color;
      }
    }

    if (snakeHead - snakeLength >= 0)
    {
      leds[snakeHead-snakeLength] = CRGB::Black;
    }


    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(10);

    // Turn our current led back to black for the next loop around

  }
}



