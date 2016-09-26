//Stuff at the top are things that are "global" variables and libraries .h (group of pre-baked funcitons) you want to use

#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

#define BRIGHTNESS  255
#define numStrip 8          //strips are contiguous physical LED chains
#define numStrand 32        //strands are the vertical "strands" that make up the chandelier
#define numLedStrip 300
#define numLedStrand 75
#define numLed 2400
uint8_t globalBrightness = 128;
uint8_t numStrandStrip = 4;


CRGB leds[numLedStrand][numStrand];
CRGB showLeds[numLed];


// code that only runs once
void setup() {
  LEDS.addLeds<OCTOWS2811>(showLeds, numLedStrip);
  LEDS.setBrightness(globalBrightness);
}



// code that runs over and over
void loop() {

  for (int led = 0 ; led < numLed ; led = led + 1)
  {
    int nextLed = led + 1200;
    int spread = 2400/32;
    int chasers = 32;
    
    for (int numChasers = 0 ; numChasers < chasers ; numChasers++)
    {
      showLeds[ (led + spread * numChasers) % numLed ] = CHSV ((led + spread * numChasers) % 255 , 255 , 150);
    }
    //    showLeds[ led ] = CHSV (led % 255 , 255 , 150);
    //    showLeds[ (led + spread) % numLed ] = CHSV ((led+spread) % 255 , 255 , 150);
    //    showLeds[ nextLed % numLed] = CHSV (led % 255 , 255 , 150);
    //    showLeds[ (nextLed + spread) % numLed ] = CHSV ((led+spread) % 255 , 255 , 150);
    //    showLeds[ (nextLed - 600) % numLed] = CHSV (led % 255 , 255 , 150);
    //    showLeds[ (nextLed - 600 + spread) % numLed ] = CHSV ((led+spread) % 255 , 255 , 150);
    //    showLeds[ (nextLed + 600) % numLed] = CHSV (led % 255 , 255 , 150);
    //    showLeds[ (nextLed + 600 + spread) % numLed ] = CHSV ((led+spread) % 255 , 255 , 150);
    FastLED.show();
    fill_solid (showLeds, numLed, CRGB::Black);
  }
}

//for ( variableName = variableStartingPoint ; variableEndingPoint ; variableChangeAmount)
//{
//  thing to do
//}
//
//for (int i = 0 ; i < 10 ; i = i+1 )
//{
//  thing to do
//}
