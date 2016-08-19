#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS    60
#define DATA_PIN    3
#define COLOR_ORDER GRB
#define CHIPSET     NEOPIXEL

CRGB leds[NUM_LEDS];
int snakeLength = 100;
#define color White;
int scaler = 1;
long firstTime = 0;
int frequency = 8353;
int loops = 0;

// This function sets up the ledsand tells the controller about them
void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(255);
  Serial.begin(115200);
  delay(10);
  Serial.flush();
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop()
{
  
  if (loops == 0)
  {
    while(!Serial.available())
    {
      firstTime = millis();
    }
    lightning();
  }
  if ( ((millis() - firstTime) % frequency) == 0 ) lightning();

}

void lightning() 
{
  for (int loopCounter = 0; loopCounter < random8(3,8); loopCounter++)
  {
    if(loopCounter == 0) scaler = 3;
    else scaler = random8(1,3);
    // Move a single white led
    for(int i = 0; i <NUM_LEDS; i++){
      leds[i].r = 255/scaler;
      leds[i].g = 255/scaler;
      leds[i].b = 255/scaler;
    }

    FastLED.show();
    delay(random8(4,10));

    for(int i = 0; i <NUM_LEDS; i++){
      leds[i].r = 0;
      leds[i].g = 0;
      leds[i].b = 0;
    }
    FastLED.show();

    if (loopCounter == 0) delay (200); //longer delay of first duller bolt
    else delay (50+random8(100)); //delay between strong bolts
  }
  //delay(1000 + random8()*10); //delay between strikes
  loops++;
}













