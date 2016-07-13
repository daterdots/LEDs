//  TwinkleSparkle2014 is a program that lets you make an LED strip
//  "twinkle" all the time and "sparkle" at a defined interval
//
//  Daniel Wilson, 2014
//
//  With BIG thanks to the FastLED community and especially Mark
//  Kriegsman whose "Fire2012" simulation inspired this program.
//  Some of Mark's original code and comments persist in this work.
///////////////////////////////////////////////////////////////////////////////////////////
#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

void Twinkle();
void Sparkle();

#define numLed_PER_STRIP 300
#define NUM_STRIPS 8
#define numLed  2400
//#define DATA_PIN           3
//#define CHIPSET            NEOPIXEL  // make sure you enter the correct chipset
//#define COLOR_ORDER        GRB       // and RBG order
#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  30
#define COOLING            3         // controls how quickly LEDs dim
#define TWINKLING          50       // controls how many new LEDs twinkle
#define FLICKER            50        // controls how "flickery" each individual LED is

CRGB leds[NUM_STRIPS * numLed_PER_STRIP];

unsigned int seeds =       0;

static int heat[numLed];
//#define Serial Serial1
///////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(3000);
  LEDS.addLeds<OCTOWS2811>(leds, numLed_PER_STRIP).setCorrection( TypicalSMD5050 );
  LEDS.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  //Serial.flush();
  //Serial.println("test");
  //while ( Serial.available() ) Serial.read(); // this helps to clear out any junk in the UART
}

void fadeall(uint8_t fading) {
  for (int i = 0; i < numLed; i++) {
    leds[i].nscale8(255 - fading);
  }
}

CHSV TwinkleColor( int temperature)
{
  CHSV heatcolor;
  heatcolor.hue = 40;
  heatcolor.saturation = 175;
  heatcolor.value = temperature;
  return heatcolor;
}

void loop()
{
  Twinkle();
  Serial.println("test");
  if (Serial.available() > 0) {
    Serial.println("sparkle");
    while (Serial.available() > 0) {
      Serial.read();
    }
    Sparkle();
  }
  FastLED.show(); // display this frame
}

void Twinkle()
{
  // Step 1. Create a randome number of seeds
  random16_add_entropy( random()); //random16() isn't very random, so this mixes things up a bit
  seeds = random16(10, numLed - 10); //the seeds change how many sites will get added heat

  // Step 2. "Cool" down every location on the strip a little
  for ( int i = 0; i < numLed; i++) {
    heat[i] = qsub8( heat[i], COOLING);
  }

  // Step 3. Make the seeds into heat on the string
  for ( int j = 0 ; j < seeds ; j++) {
    if (random16() < TWINKLING) {
      //again, we have to mix things up so the same locations don't always light up
      random16_add_entropy( random());
      heat[random16(numLed)] = random16(50, 255);
    }
  }

  // Step 4. Add some "flicker" to LEDs that are already lit
  //         Note: this is most visible in dim LEDs
  for ( int k = 0 ; k < numLed ; k++ ) {
    if (heat[k] > 0 && random8() < FLICKER) {
      heat[k] = qadd8(heat[k] , 10);
    }
  }

  // Step 5. Map from heat cells to LED colors
  for ( int j = 0; j < numLed; j++)
  {
    leds[j] = TwinkleColor( heat[j] );
  }

}

// Sparkle works very much like Twinkle, but with more LEDs lighting up at once
void Sparkle() {
  for (int sparkles = 0 ; sparkles < 5 ; sparkles ++) {
    // Step 1. Make a random numnber of seeds
    seeds = random16( 0  , numLed / 2);

    // Step 2. Increase the heat at those locations
    for ( int i = 0 ; i < seeds ; i++) {
      {
        int pos = random16(numLed);
        random16_add_entropy( random());
        heat[pos] = qadd8(heat[pos], random8(50, 100));
      }
    }

    random16_add_entropy( random()); //random8() isn't very random, so this mixes things up a bit
    //seeds = random16(10, numLed - 10);

    // Step 2. "Cool" down every location on the strip a little
    for ( int i = 0; i < numLed; i++) {
      heat[i] = qsub8( heat[i], COOLING);
    }

    // Step 3. Make the seeds into heat on the string
    for ( int j = 0 ; j < seeds ; j++) {
      if (random16() < TWINKLING) {
        //again, we have to mix things up so the same locations don't always light up
        random16_add_entropy( random());
        heat[random16(numLed)] = random16(50, 255);
      }
    }

    // Step 4. Add some "flicker" to LEDs that are already lit
    //         Note: this is most visible in dim LEDs
    for ( int k = 0 ; k < numLed ; k++ ) {
      if (heat[k] > 0 && random8() < FLICKER) {
        heat[k] = qadd8(heat[k] , 10);
      }
    }

    // Step 5. Map from heat cells to LED colors
    for ( int j = 0; j < numLed; j++)
    {
      leds[j] = TwinkleColor( heat[j] );
    }
    FastLED.show();
    //delay(1);
  }
}
