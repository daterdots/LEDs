/*
   Chandelier2016 by Daniel Wilson
*/

#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>
#include <Audio.h>
//#include <Wire.h>
//#include <SD.h>
//#include <SPI.h>

// global definitions
#define numStrip 8          //strips are contiguous physical LED chains
#define numStrand 32        //strands are the vertical "strands" that make up the chandelier
#define numLedStrip 300
#define numLedStrand 75
#define numLed 2400
#define led 13
//#define serial Serial
uint8_t numStrandStrip = 4;

void runCommand(char);
void runProgram();
void blinker();
void winker();

uint8_t brightness = 16;
uint8_t delayTime = 128;
uint8_t* currentKnob = &brightness;

void (*currentProgram)();

CRGB currentColor = CRGB::Purple ;

char mode = 'a';
int levelForget = 1000;
float levels[numStrand] = {0};
float relativeLevels[numStrand] = {0};
float peakLevel = 0.0;

//Audio library objects
AudioInputAnalog         adc1(A3);       //xy=99,55
AudioAnalyzeFFT1024      fft;            //xy=265,75
AudioConnection          patchCord1(adc1, fft);
int frequencyBinsHorizontal[numStrand + 1] = {
  1,   1,  1,  1,  1,  1,  2,  2,
  2,   3,  3,  4,  5,  5,  6,  7,
  8,  10, 11, 13, 15, 17, 20, 23,
  26, 30, 35, 40, 46, 53, 61, 70
};

//define the leds as a matrix for doing animations, then as an array for final display
CRGB leds[numLedStrand][numStrand];
CRGB tempLeds[numLedStrand][numStrand];
CRGB showLeds[numLed];
CRGB SparklerColor(int temperature);

void setup() {
  currentProgram = blinker;
  AudioMemory(12);
  LEDS.addLeds<OCTOWS2811>(showLeds, numLedStrip);
  LEDS.setBrightness(brightness);
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  for ( int i = 0 ; i < 10 ; i++ )
  {
    digitalWrite(led, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(led, LOW);   // turn the LED off by making the voltage LOW
    delay(100);               // wait for a second
  }
  //delay(3000);                //sanity delay for reprogramming
}

void loop() {

  /* Command Ideas
      help 'h'
      change program
      initiate intra-program stuff ' ; ' ' ' '
      change brightness 'b'
      change dimming rate 'd'
      change "noise" rate 'n'
      change the forgetfullness of the loudest sound 'f'
  */

  while ( Serial.available() > 0 )
  {
    runCommand( Serial.read() );
  }

  Serial.println(brightness);
  Serial.println("test");

  runProgram();

}

void runCommand(char command)
{
  if ( command == '+' || command ==  '=' )
  {
    *currentKnob += 16;
    Serial.println("turn it up");
  }

  else if ( command == '-' || command ==  '_' )
  {
    *currentKnob -= 16;
    Serial.println("turn it down");
  }

  else if ( command == 'd' ) //d for delayTime
  {
    currentKnob = &delayTime; // change where the pointer points
  }

  else if ( command == 'b' )
  {
    currentKnob = &brightness; // change where the pointer points
  }

  else if ( command == 'B' )
  {
    currentProgram = &blinker;
  }
  else if ( command == 'W' )
  {
    currentProgram = &winker;
  }
}

void runProgram()
{
  (*currentProgram)();
  LEDS.setBrightness(brightness);
  FastLED.show();
}

void blinker()
{
  Serial.println("In blinker");
  fill_solid(showLeds, numLed, currentColor);
  delay(delayTime);
  FastLED.show();
  fill_solid(showLeds, numLed, CRGB::Black);
  delay(delayTime);
  FastLED.show();
}

void winker()
{
  Serial.println("In blinker");
  fill_solid(showLeds, numLed, currentColor);
  delay(delayTime);
  FastLED.show();
  fill_solid(showLeds, numLed, CRGB::White);
  delay(delayTime);
  FastLED.show();
}


