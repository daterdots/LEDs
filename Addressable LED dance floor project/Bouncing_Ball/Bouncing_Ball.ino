#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS          60
#define DATA_PIN          3
#define COLOR_ORDER       GRB
#define CHIPSET           NEOPIXEL
#define GRAVITY           -10            // m/s^2
#define STRIP_LENGTH      1              // m

float h =               0 ;  // start at the top of the strip
int x0 =               1 ;
int v0 =               0 ;
float v =                 0 ;
int dir =                 -1 ;            // -1 for down, 1 for up
int cycle =               0 ;

float vImpact = sqrt( 2 * GRAVITY * ( 0 - x0 ) );
int tImpact = -1 * vImpact/GRAVITY * 1000;
float tCycle = 0 ; 
int pos = 0 ;

CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(255);
  Serial.begin(115200);
}

void loop() {
  Serial.println("top of loop ");
  cycle = floor( millis() / tImpact ) ;
  tCycle = millis() % tImpact ;

  if ( cycle % 2 == 0 ) {   // if cycle is an even numer the ball is going down
    h = 0.5 * GRAVITY * pow( tCycle/1000 , 2.0 ) + x0;
    pos = round( h * (NUM_LEDS - 1));
  }

  else {   // if cycle is an odd numer then ball is going up
    h = 0.5 * GRAVITY * pow( tCycle/1000 , 2.0 ) + vImpact * tCycle/1000;
    pos = round( h * (NUM_LEDS - 1) );
  }

  /*Serial.print("tCycle is ");
  Serial.println(tCycle);

  Serial.print("cycle is ");
  Serial.println(cycle);*/

  Serial.print("h is ");
  Serial.println(h);
  Serial.println();
  
    Serial.print("pos is ");
  Serial.println(pos);
  Serial.println();
  
  
  Serial.println("before LED commands ");
    //Turn the LED on, then pause
  leds[pos] = CRGB::Red;
  FastLED.show();
    //delay(50);
  // Now turn the LED off, then pause
  leds[pos] = CRGB::Black;
  Serial.println("end of loop ");



}





