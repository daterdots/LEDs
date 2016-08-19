#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS           60
#define DATA_PIN           3
#define COLOR_ORDER        GRB
#define CHIPSET            NEOPIXEL
#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  30
#define SPARKING           5
#define TWINKLING          100
#define COOLING            15
#define GRAVITY            1
#define TWINKLE_RADIUS     1
#define FIREWORK_RADIUS    5

static int FIREWORK_DIA =  2*FIREWORK_RADIUS + 1;
CRGB leds[NUM_LEDS];

// This function sets up the ledsand tells the controller about them
void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() 
{
  random16_add_entropy( random());
  sparkle2014(); // run simulation frame
  FastLED.show(); // display this frame

#if defined(FASTLED_VERSION) && (FASTLED_VERSION >= 2001000)
  FastLED.delay(1000 / FRAMES_PER_SECOND);
#else  
  delay(1000 / FRAMES_PER_SECOND);
#endif  ﻿
}

void sparkle2014()
{
  static byte heat[NUM_LEDS];
  static byte heatLast[NUM_LEDS];
  static byte rising[NUM_LEDS];
  static byte risingLast[NUM_LEDS];

  //make seeds
  if (random8() < 10) {
    random16_add_entropy( random());
    int seedLed = random8(NUM_LEDS);
    heat[seedLed] = random8();
  }
  
    //cool everybody
  for( int k = 0; k < NUM_LEDS ; k++) 
  {
    heat[k] = heat[k] - 3;
  }


  for( int i = 0 ; i < NUM_LEDS ; i ++) {
    heatLast[i] = heat[i];
    risingLast[i] = rising[i];
  }
  
  //increase heat based on neightbors' around the seed
  for( int k = 0; k < NUM_LEDS ; k++) 
  {
    if ( heatLast[k - 1] && heatLast[k + 1] < heatLast[k] ) {
      rising[k] = 1;
      heat[k -1] = qadd8( heatLast[k] , 10/(heatLast[k]/10 + 1) + 1) ;
      heat[k +1] = qadd8( heatLast[k] , 10/(heatLast[k]/10 + 1) + 1) ;
      if( heat[k] == 255) heat [k] = 244;
    }
    if (heatLast[k - 1] && heatLast[k + 1] < heatLast[k]) {
      rising[k] = -1;
    }
    else {
      rising[k] = 0;
    }
  }

  //
  for( int j = 0; j < NUM_LEDS; j++) 
  {
    leds[j] = HeatColor( heat[j]);
  }

}

CRGB HeatColor( uint8_t temperature) {
  CRGB heatcolor;

  // Scale 'heat' down from 0-255 to 0-191,
  // which can then be easily divided into three
  // equal 'thirds' of 64 units each.
  uint8_t t192 = scale8_video( temperature, 192);

  // calculate a value that ramps up from
  // zero to 255 in each 'third' of the scale.
  uint8_t heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

    // now figure out which third of the spectrum we're in:

  if( t192 & 0x80) {
    // we're in the hottest third
    heatcolor.r = 255; // full red
    heatcolor.g = heatramp; // full green
    heatcolor.b = 255; // ramp up blue

  } 
  else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = 255; // full red
    heatcolor.g = 0; // ramp up green
    heatcolor.b = heatramp; // no blue

  } 
  else {
    // we're in the coolest third
    heatcolor.r = heatramp; // ramp up red
    heatcolor.g = 0; // no green
    heatcolor.b = 0; // no blue
  }
  return heatcolor;
}








