#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS           60
#define DATA_PIN           3
#define COLOR_ORDER        GRB
#define CHIPSET            NEOPIXEL
#define BRIGHTNESS         100
#define FRAMES_PER_SECOND  500


CRGB leds[NUM_LEDS];

void setup() 
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
}

void loop()
{
  rainbow(1000/FRAMES_PER_SECOND);
}

void rainbow(uint8_t wait) 
{
  uint16_t hue;
  FastLED.clear();

  for(hue = 0; hue < 256 * 3; hue++) 
  {
    fill_rainbow( &(leds[0]), NUM_LEDS, hue /*starting hue*/);		
    FastLED.show();
    delay(wait);
  }
  return;
}

