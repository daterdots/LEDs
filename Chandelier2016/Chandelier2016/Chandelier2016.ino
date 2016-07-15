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
uint8_t numStrandStrip = 4;
uint8_t globalBrightness = 128;
char mode = 'a';
int levelForget = 1000;
float levels[numStrand] = {0};
float relativeLevels[numStrand] = {0};
float peakLevel = 0.0;

//prototype functions
void transform(CRGB ledMatrix[numLedStrand][numStrand]);
void columnsAndRows();
void rainbowColumns();
void spectrumSparkle();
void america();
void spiral();
void hula();
void spin();
float maxarr(float arr[]);
void fadeleds();

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

//America
#define SPARKLER_SPARKING           240
#define SPARKLER_CHILL_SPARKING     10
#define SPARKLER_COOLING            200

//Spectrum Analyzer
int spins = 0;
int theta = 0;
float maxLevel = 0.0 ;
unsigned int spinSpeed = 5;//inversely proportional to spin speed

//define the leds as a matrix for doing animations, then as an array for final display
CRGB leds[numLedStrand][numStrand];
CRGB tempLeds[numLedStrand][numStrand];
CRGB showLeds[numLed];
CRGB SparklerColor(int temperature);

// Pin layouts on the teensy 3:
// OctoWS2811: 2,14,7,8,6,20,21,5

//////// temp variables
int a = 0;
////////

void setup()
{
  AudioMemory(12);
  LEDS.addLeds<OCTOWS2811>(showLeds, numLedStrip);
  LEDS.setBrightness(globalBrightness);
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  for ( int i = 0 ; i < 10 ; i++ )
  {
    digitalWrite(led, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(led, LOW);   // turn the LED off by making the voltage LOW
    delay(100);               // wait for a second
  }
  delay(3000);                //sanity delay for reprogramming
}

void loop()
{
  while ( Serial.available() > 0 )
  {
    mode = Serial.read();
  }

  checkProgram();

  //columnsAndRows();
  //rainbowColumns();
  //america();
  //spiral();
  //hula();
}

void columnsAndRows() {
  for (int column = 0; column < numStrand; column++)
  {
    for (int row = 0; row < numLedStrand; row++)
    {
      leds[row][column] = CRGB::White;
      transform(leds);
      FastLED.show();
      //delay(10);
      leds[row][column] = CRGB::Black;
      fill_solid(showLeds, numLed, CRGB::Black);
      FastLED.show();
      //delay(20);
    }
  }

  for (int row = 0; row < numLedStrand; row++)
  {
    for (int column = 0; column < numStrand; column++)
    {
      leds[row][column] = CRGB::White;
      transform(leds);
      FastLED.show();
      //delay(10);
      leds[row][column] = CRGB::Black;
      fill_solid(showLeds, numLed, CRGB::Black);
      //delay(10);

    }

  }
}

void rainbowColumns()
{
  for (int hue = 255 ; hue >=  0 ; hue --) {
    for (int column = 0; column < numStrand; column++)
    {
      CRGB tempLeds[numLedStrand];
      fill_rainbow (tempLeds, numLedStrand, hue, 10 );
      for ( int row = 0; row < numLedStrand; row++ )
      {
        leds[row][column] = tempLeds[row];
      }
    }
    transform(leds);
    FastLED.show();
    //delay(10);
  }
}

void america()
{
  static uint16_t heat[numLed];

  random16_add_entropy( random());
  FastLED.show(); // display this frame
  //delay(20);

  //Cool down every cell a little
  for ( int i = 0; i < numLed; i++) {
    heat[i] = qsub8( heat[i], SPARKLER_COOLING / 255 + 1);
  }

  // Randomly ignite new 'sparks'
  if ( random8() / 8 < SPARKLER_SPARKING ) {
    random16_add_entropy( random());
    int y = random16(numLed); //y is the position, make sparks in this range
    heat[y] = qadd8( heat[y], random8(150, 255) ) - 1 ; //the -1 is here because of the 255 == yellow bug
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < numLed; j++)
  {
    showLeds[j] = SparklerColor(heat[j]);
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
  if ( t192 & 0x80) {
    // we're in the hottest third
    heatcolor.r = heatramp; // full red
    heatcolor.g = heatramp; // full green
    heatcolor.b = heatramp; // ramp up blue

  }
  else if ( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = heatramp; // full red
    heatcolor.g = heatramp * 75 / 255; // ramp up green
    heatcolor.b = heatramp * 25 / 255; // no blue

  }
  else {
    // we're in the coolest third
    heatcolor.r = heatramp * 20 / 255; // ramp up red
    heatcolor.g = heatramp * 50 / 255; // no green
    heatcolor.b = heatramp; // no blue
  }
  return heatcolor;
}

void spiral()
{
  for ( int column = 0 ; column < numStrand ; column++)
  {
    for ( int row = 0 ; row < numLedStrand ; row++)
    {
      leds[row][column] = CRGB::White ;
    }
    transform(leds);
    FastLED.show();
    delay(10);
    for ( int row = 0 ; row < numLedStrand ; row++)
    {
      leds[row][column] = CRGB::Black ;
    }
    //transform();
    //FastLED.show();
    //delay(100);
  }
}

void hula()
{
  for ( int row = 0 ; row < numLedStrand ; row++)
  {
    for ( int column = 0 ; column < numStrand ; column++)
    {
      leds[row][column] = CHSV(row, 255, 255) ;
    }

    for ( int row = 0 ; row < numLedStrand ; row++)
    {
      for ( int column = 0 ; column < numStrand ; column++)
      {
        leds[row][column].nscale8(200);
      }
    }
    transform(leds);
    FastLED.show();
  }

  for ( int row = numLedStrand - 1 ; row >= 0 ; row--)
  {
    for ( int column = 0 ; column < numStrand ; column++)
    {
      leds[row][column] = CHSV(row, 255, 255) ;
    }
    for ( int row = 0 ; row < numLedStrand ; row++)
    {
      for ( int column = 0 ; column < numStrand ; column++)
      {
        leds[row][column].nscale8(200);
      }
    }
    transform(leds);
    FastLED.show();
  }
}

void spectrumSparkle() {
  for ( theta = 0 ; theta < numStrand * spinSpeed ; theta ++)
  {
    unsigned int freqBin, x, y ;
    float level;

    if (fft.available())
    {
      freqBin = 2; //ignore the first two bins which contain DC offsets
      for (x = 0; x < numStrand; x++) {
        levels[x] = fft.read(freqBin, freqBin + frequencyBinsHorizontal[x] - 1);
        freqBin = freqBin + frequencyBinsHorizontal[x];
      }
    }

    maxLevel = maxarr(levels) ;
    if ( maxLevel > peakLevel)
    {
      peakLevel = maxLevel;
    }

    Serial.print("max level = ");
    Serial.println(maxLevel);
    Serial.print("peak level = ");
    Serial.println(peakLevel);

    //relativeLevels = levels / peakLevel ;
    for (x = 0; x < numStrand; x++) {
      relativeLevels[x] = levels[x] / peakLevel ;
    }
    peakLevel = peakLevel - peakLevel / levelForget;

    //printFFT();

    for (int row = 0 ; row < numLedStrand ; row++ )
    {
      for (int column = 0 ; column < numStrand / 2 ; column++ )
      {
        if ( ((relativeLevels[2 * column] + relativeLevels[2 * column + 1]) / 2 * numLedStrand) > row)
        {
          leds[numLedStrand - row - 1][column] = CHSV(column * 16 - 8 , 255 - 3.4 * row , 255 - 3 * row);
          leds[numLedStrand - row - 1][numStrand - 1 - column] = CHSV(column * 16 - 8 , 255 - 3.4 * row , 255 - 3 * row);
        }
        else
        {
          if (random8(maxLevel / peakLevel * 255, 255) > 253  && random8(maxLevel / peakLevel * 255, 255) > 253 && random8() > 250)
          {
            leds[numLedStrand - row - 1][column] += CHSV(0, 0, random8(150, 255)) ;
            leds[numLedStrand - row - 1][numStrand - 1 - column] += CHSV(0, 0, random8(150, 255)) ;
          }
          else {}
        }
      }
    }
    
    spin();
    transform(tempLeds);
    FastLED.show();
    fadeleds();
  }
}

void spin()
{
  for ( int row = 0 ; row < numLedStrand ; row ++ )
  {
    for ( int column = 0 ; column < numStrand ; column ++ )
    {
      int newColumn = (column + theta / spinSpeed) % numStrand;
      tempLeds[row][newColumn] = leds[row][column];
    }
  }
}

void transform(CRGB ledMatrix[numLedStrand][numStrand])
{
  for (int column = 0; column < numStrand; column++)
  {
    int strip = floor(column / numStrandStrip);
    for (int row = 0; row < numLedStrand; row++)
    {
      if ((column - strip * numStrandStrip) % 2 == 1) //check to see if this is the strip that goes bottom to top
      {
        showLeds[column * numLedStrand + row] = ledMatrix[numLedStrand - row - 1][column];
      }
      else
      {
        showLeds[column * numLedStrand + row] = ledMatrix[row][column];
      }
    }
  }
}

void fadeleds()
{
  for (int row = 0; row < numLedStrand; row++) {
    for (int column = 0; column < numStrand; column++) {
      leds[row][column].nscale8(245);
    }
  }
}

void checkProgram() {
  switch (mode)
  {
    case c:
      cylon();
      break;

    case 1:
      stochasticChill();
      break;

  }
}
