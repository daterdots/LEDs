#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LEDS    60
#define DATA_PIN    3
#define COLOR_ORDER GRB
#define CHIPSET     NEOPIXEL

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 40

CRGB leds[NUM_LEDS];


long current = 1;
long past = 1;
long temp = 1;
long count = 0;
int fibonacciMax = 100;
int scaler = 1;

void setup() 
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  Serial.begin(115200);
  Serial.flush();
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );

}

void loop() 
{
  checkSerial();
}

/******************************************************************/
void checkSerial()
{
  //if there is nothing in the serial port, go back to checkSerial
  if(!Serial.available()) 
  { 
    return;   
  }

  char command = Serial.read();
  Serial.print("you just typed: "); 
  Serial.println(command); 

  switch(command)
  {
  case 'a': 
    Serial.println("awesome"); 
    Serial.print('\n');  
    break;

  case 'b':
    Serial.println("badass");
    Serial.print('\n');  
    break;

  case 'c':
    Serial.println("crap");
    Serial.print('\n');  
    break;

  case 'f':
    Serial.println("Welcome to the Fibonacci Sequence");
    fibonacci();
    current = 1;
    past = 1;
    temp = 1;
    count = 0;
    break;

  case 'l':
    Serial.println("lightning");
    lightning();
    break;

  case '!':
    Serial.println("fire");
    fire();
    break;


  case 'r':
    redMover();
    break;

  case '?':
    helpMenu();
    break;

  default:
    Serial.println("Unknown command. Try these:");
    Serial.print('\n');  
    helpMenu();
    break;
  }
}
/******************************************************************/
void helpMenu()
{
  Serial.println("**********************");
  Serial.println("THIS IS THE HELP MENU");
  Serial.println("**********************");
  Serial.println(" a - this makes things do a-wesome stuff");
  Serial.println(" b - whoa, this is totally b-adass");
  Serial.println(" c - I am serioulsy c-rapping my pants right now");
  Serial.println(" d - ");
  Serial.println(" e - ");
  Serial.println(" f - you just got Fibonaccied, sucka!");
  Serial.println(" g - ");
  Serial.println(" h - ");
  Serial.println(" i - ");
  Serial.println(" r - a r-ed dot walks down the strip");
  Serial.print('\n');
}
/******************************************************************/
void fibonacci() 
{
  //Serial.print("The value of the sequence is now ");
  //Serial.println(current);
  while(current<fibonacciMax && !Serial.available())
  {
    if(current==1)
    {
      delay(500);
      Serial.print("\n");
      Serial.println("start with 1");
    }

    while (count<current)
    {
      Serial.print("|"); 
      delay (20);
      count++;
    }

    temp = current;
    current += past;
    past = temp;
    count = 0;

    Serial.print("\n");
    Serial.print("\n");

    if(current<fibonacciMax)
    {
      delay(500);
      Serial.print(past);
      Serial.print(" + ");
      Serial.print(current-past);
      Serial.print(" = ");
      Serial.println(current);
    }
  }
}
/******************************************************************/
void redMover()
{
  while(!Serial.available()) 
  {
    for(int dot = 0; dot < NUM_LEDS; dot++) 
    { 
      leds[dot] = CRGB::Red;
      FastLED.show();
      // clear this LED for the next time around the loop
      leds[dot] = CRGB::Black;
      delay(30);
    }

  } 
}
/******************************************************************/
void lightning() 
{
  //while(!Serial.available()) 
  {
    for (int loopCounter = 0; loopCounter < random8(2,8); loopCounter++)
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
    delay(1000 + random8()*10); //delay between strikes
  }
}
/******************************************************************/

void fire()
{
  while(!Serial.available()) 

  {
    // Add entropy to random number generator; we use a lot of it.
    random16_add_entropy( random());

    Fire2012(); // run simulation frame
    FastLED.show(); // display this frame

#if defined(FASTLED_VERSION) && (FASTLED_VERSION >= 2001000)
    FastLED.delay(1000 / FRAMES_PER_SECOND);
#else  
    delay(1000 / FRAMES_PER_SECOND);
#endif  ﻿
  }
}
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  75

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 150

void Fire2012()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= 0; k < NUM_LEDS - 3; k++) {
    heat[k] = (heat[k + 1] + heat[k + 2] + heat[k + 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < SPARKING ) {
    int y = random8(NUM_LEDS-7,NUM_LEDS);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
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
    heatcolor.g = 255; // full green
    heatcolor.b = heatramp; // ramp up blue

  } 
  else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = 255; // full red
    heatcolor.g = heatramp; // ramp up green
    heatcolor.b = 0; // no blue

  } 
  else {
    // we're in the coolest third
    heatcolor.r = heatramp; // ramp up red
    heatcolor.g = 0; // no green
    heatcolor.b = 0; // no blue
  }

  return heatcolor;
}
/******************************************************************/



























