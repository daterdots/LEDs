#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

#define NUM_LEDS_PER_STRIP 150
#define NUM_STRIPS 2
long NUM_LEDS = NUM_STRIPS * NUM_LEDS_PER_STRIP;
uint8_t strands_per_strip = 2;
uint8_t strand_lenth = 75;

CRGB leds[NUM_LEDS_PER_STRIP * NUM_STRIPS];
CRGB math_leds[NUM_LEDS_PER_STRIP * NUM_STRIPS];

// Pin layouts on the teensy 3:
// OctoWS2811: 2,14,7,8,6,20,21,5

void setup() {
  LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  LEDS.setBrightness(10);
  Serial.begin(9600);
}

void loop() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)
  {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::White;

    for ( int strip = 0; strip < NUM_STRIPS; strip++)
    {
      for ( int strand = 0; strand < strands_per_strip; strand++)
      {
        if (strand % 2 == 1)
        {
          for ( int pixel = 0; pixel < strand_lenth; pixel++)
          {
            math_leds[ pixel + (strand * strand_lenth) + (strip * NUM_LEDS_PER_STRIP) ] = leds[ ((strand * strand_lenth) + (strip * NUM_LEDS_PER_STRIP)) + (strand_lenth - pixel) ];
          }
        }
        else
        {
          for ( int pixel = 0; pixel < strand_lenth; pixel++)
          {
            math_leds[ pixel + (strand * strand_lenth) + (strip * NUM_LEDS_PER_STRIP) ] = leds[ pixel + (strand * strand_lenth) + (strip * NUM_LEDS_PER_STRIP) ];
          }
        }
      }
    }

    for ( int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = math_leds[i];
    }

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(20);

    // Turn our current led back to black for the next loop around
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
}

//void yourfunction(byte array)
//{
//  for ( int index = startarray[led_strand]; index < startarray[led_strand] + lengtharray[led_strand]; index++)
//  {
//    //do your thing
//  }
//  FastLED.show();
//}


