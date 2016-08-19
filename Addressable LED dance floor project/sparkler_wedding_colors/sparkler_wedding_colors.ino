#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS           120
#define DATA_PIN           3
#define COLOR_ORDER        GRB
#define CHIPSET            NEOPIXEL
#define BRIGHTNESS         50
#define FRAMES_PER_SECOND  3000
#define SPARKLER_SPARKING           10
#define SPARKLER_COOLING            100 
#define GRAVITY            1

CRGB leds[NUM_LEDS];

#define color White;
int scaler = 1;
int loc = 0;

// This function sets up the ledsand tells the controller about them
void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() 
{
  random16_add_entropy( random());
  //random16_add_entropy( random());
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

  //Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i], COOLING / 255 + 1);
  }

  // Randomly ignite new 'sparks'
  if( random8() < SPARKING ) {
    random16_add_entropy( random());
    int y = random8(NUM_LEDS); //y is the position, make sparks in this range
    heat[y] = qadd8( heat[y], random8(150,255) ) -1 ; //the -1 is here because of the 255 == yellow bug
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < NUM_LEDS; j++) 
  {
    leds[j] = SparklerColor( heat[j]);
  }
}

CRGB SparklerColor(int temperature)
{
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
    heatcolor.r = heatramp; // full red
    heatcolor.g = heatramp; // full green
    heatcolor.b = heatramp; // ramp up blue

  } 
  else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = heatramp; // full red
    heatcolor.g = heatramp * 75/255; // ramp up green
    heatcolor.b = heatramp * 30/255; // no blue

  } 
  else {
    // we're in the coolest third
    heatcolor.r = heatramp * 40 / 255; // ramp up red
    heatcolor.g = heatramp * 130 / 255; // no green
    heatcolor.b = heatramp; // no blue
  }
  return heatcolor;
}


























