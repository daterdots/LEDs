#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS           60
#define DATA_PIN           3
#define COLOR_ORDER        GRB
#define CHIPSET            NEOPIXEL
#define BRIGHTNESS         50
#define FRAMES_PER_SECOND  30
#define SPARKING           200
#define COOLING            25
#define GRAVITY            1

CRGB leds[NUM_LEDS];

#define color White;
int scaler = 1;
int loc = 0;

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

  //Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  heat[i] * COOLING / 255 + 1);
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k = NUM_LEDS - 1; k >= 0; k--) {
    heat[k] = heat[ (2 * k - 1) - round(pow(k,1.01))];
  }

  // Randomly ignite new 'sparks'
  if( random8() < SPARKING ) {
    int y = random8(0,10); //y is the position, make sparks in this range
    heat[y] = qadd8( heat[y], random8(150,255) ) -1 ; //the -1 is here because of the 255 == yellow bug
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < NUM_LEDS; j++) 
  {
    leds[j] = HeatColor( heat[j]);
  }
}

CRGB HeatColor( uint8_t temperature)
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
    heatcolor.r = 255; // full red
    heatcolor.g = heatramp; // full green
    heatcolor.b = 255; // ramp up blue

  } 
  else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = heatramp; // full red
    heatcolor.g = 0; // ramp up green
    heatcolor.b = 255; // no blue

  } 
  else {
    // we're in the coolest third
    heatcolor.r = 0; // ramp up red
    heatcolor.g = 0; // no green
    heatcolor.b = heatramp; // no blue
  }
  return heatcolor;
}

/*
loc = random8(NUM_LEDS);
 Serial.print(loc);
 // Turn the LED on, then pause
 leds[loc] = CRGB::White;
 FastLED.show();
 //delay(1);
 // Now turn the LED off, then pause
 leds[loc] = CRGB::Black;
 FastLED.show();
 //delay(1); 
 */



























