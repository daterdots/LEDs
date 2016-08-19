#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_STRIPS         3
#define NUM_LEDS_MAX       60

#define DATA_PIN_1         3
#define DATA_PIN_2         4
#define DATA_PIN_3         5

#define COLOR_ORDER        GRB
#define CHIPSET            NEOPIXEL
#define BRIGHTNESS         100
#define FRAMES_PER_SECOND  500

int NUM_LEDS[] = { 
  60 , 30 };
int DATA_PIN[] = {  
  3 ,  4 };

uint16_t hue = 0;



CRGB leds[NUM_STRIPS][100];

void setup() 
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, 3, COLOR_ORDER>(leds[0], NUM_LEDS[0]);
  FastLED.addLeds<CHIPSET, 4, COLOR_ORDER>(leds[1], NUM_LEDS[1]);
  LEDS.setBrightness(BRIGHTNESS);
}

void loop()
{
  rainbow(1000/FRAMES_PER_SECOND);
}

void rainbow(uint8_t wait) 
{
  for(int x = 0; x < NUM_STRIPS; x++) 
  {
    fill_rainbow( &(leds[x][0]), NUM_LEDS[x], hue /*starting hue*/);		
    FastLED.show();
    delay(wait);
  }
  hue == 255 ? hue = 0 : hue++ ;
  return;
}






