
void computeVerticalLevels();

void transformTemp();
void black();
void spin();

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <math.h>

int freqBin = 0;
//float relativeLevel = 0;


#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>



// The display size and color to use
const unsigned int matrix_width = 32;
const unsigned int matrix_height = 75;
const unsigned int myColor = 0x400020;

void transform(CRGB ledMatrix[matrix_height][matrix_width]);


#define BRIGHTNESS  255
#define numStrip 8          //strips are contiguous physical LED chains#define numStrand 32        //strands are the vertical "strands" that make up the chandelier
#define numLedStrip 300
#define numLedStrand 75
#define numLed 2400
#define numStrand 32
uint8_t globalBrightness = 128;
uint8_t numStrandStrip = 4;

int spins = 0;
int theta = 0;
unsigned int spinSpeed = 4; //inversely proportional to spin speed
float peakLevel = 0.0;
float relativeLevel = 0.0;
float peakBassLevel = 0.0;
float relativeBassLevel = 0.0;
float peakMidLevel = 0.0;
float relativeMidLevel = 0.0;
float peakTrebLevel = 0.0;
float relativeTrebLevel = 0.0;

int levelForget = 10000;

CRGB leds[numLedStrand][numStrand];
CRGB tempLeds[numLedStrand][numStrand];
CRGB showLeds[numLed];
CRGB SparklerColor(int temperature);

// These parameters adjust the vertical thresholds
const float maxLevel = 0.1;      // 1.0 = max, lower is more "sensitive"
const float dynamicRange = 70.0; // total range to display, in decibels
const float linearBlend = 0.3;   // useful range is 0 to 0.7

// Audio library objects
AudioInputAnalog         adc1(A3);       //xy=99,55
AudioAnalyzeFFT1024      fft;            //xy=265,75
AudioConnection          patchCord1(adc1, fft);

// This array holds the volume level (0 to 1.0) for each
// vertical pixel to turn on.  Computed in setup() using
// the 3 parameters above.
float thresholdVertical[matrix_height];

// This array specifies how many of the FFT frequency bin
// to use for each horizontal pixel.  Because humans hear
// in octaves and FFT bins are linear, the low frequencies
// use a small number of bins, higher frequencies use more.
int frequencyBinsHorizontal[matrix_width + 1] = {
  1,   1,  1,  1,  1,  1,  2,  2,
  2,   3,  3,  4,  5,  5,  6,  7,
  8,  10, 11, 13, 15, 17, 20, 23,
  26, 30, 35, 40, 46, 53, 61, 70
};
//,  6,  6,  6,  7,  7,  7,  8,  8,
//9,  9, 10, 10, 11, 12, 12, 13, 14, 15,
//15, 16, 17, 18, 19, 20, 22, 23, 24, 25
//};

int timeConstants[numStrand] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2
};
int timescale = 100; // approximate voidloop steps per second
int offset = 200;
int t = 1;
int hue = 0;
int tchaos = 120 * timescale; // seconds to flip animation
int bias = 1;
// Run setup once
void setup() {
  // the audio library needs to be given memory to start working
  AudioMemory(12);

  // compute the vertical thresholds before starting
  computeVerticalLevels();

  // turn on the display
  //delay(3000);
  LEDS.addLeds<OCTOWS2811>(showLeds, numLedStrip);
  LEDS.setBrightness(globalBrightness);
  Serial1.begin(9600);


  for (int i = 0; i < numStrand; i++)
  {
    timeConstants[i] += offset;
  }

}

void fadeall()
{
  for (int i = 0; i < numLed; i++) {
    showLeds[i].nscale8(250);
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

int sinelevel = 0;

void loop() {
  for ( theta = 0 ; theta < numStrand * spinSpeed ; theta ++)
  {
    float level = 0;
    if (fft.available()) {
      freqBin = 2;
      for (int freq = 2 ; freq < 34 ; freq++ )
      {
        level = level + fft.read(freqBin, freqBin + frequencyBinsHorizontal[freq] - 1);
      }
      if (level > peakLevel)
      {
        peakLevel = level;
      }
      relativeLevel = level / peakLevel;
    }
    for (int column = 0; column < numStrand; column++)
    {
      sinelevel = (sin((t / 120.0 - PI / 2) * offset / timeConstants[column]) + 1) / 2 * numLedStrand;
      for (int row = 0; row < numLedStrand; row++)
      {
        if ( (sinelevel) == row) {
          leds[row][column] += CHSV(165, 255, 200);
        }
        //make little stars
        if (relativeLevel * 35 > random8() + random8())
        {
          leds[row][column] += CHSV(235, 240, random8(150, 255)) ;
        }
      }
    }
    peakLevel = peakLevel * .999;

    if (t == tchaos || t == 0)
    {
      bias = bias * (-1);
      //hue = 64;
    }
    t += bias;
    //Serial1.print("t = ");
    //Serial1.println(t);
    //Serial1.println();
    //spin();
    transform(leds);
    FastLED.show();
    fadeleds();
  }
}

// Run once from setup, the compute the vertical levels
void computeVerticalLevels() {
  unsigned int y;
  float n, logLevel, linearLevel;

  for (y = 0; y < matrix_height; y++) {
    n = (float)y / (float)(matrix_height - 1);
    logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
    linearLevel = 1.0 - n;
    linearLevel = linearLevel * linearBlend;
    logLevel = logLevel * (1.0 - linearBlend);
    thresholdVertical[y] = (logLevel + linearLevel) * maxLevel;
  }
}


void transform(CRGB ledMatrix[matrix_height][matrix_width])
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

void black()
{
  for (int i = 0 ; i < numLed ; i++)
  {
    showLeds[i] = CRGB::Black;
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
      //.nscale8(150);
      //Serial1.println(remainder);
      //tempLeds[row][newColumnFloor] += leds[row][column].nscale8(remainder);
      //tempLeds[row][newColumnCeil] += leds[row][column].nscale8(255 - remainder);
      //tempLeds[row][int(ceil(newColumn))] += leds[row][column].nscale8((1 - (newColumn - long(newColumn))) * 255);
      //float turd = column + theta / spinSpeed;
      //float newColumn = fmod(turd , numStrand);
      //Serial1.print("new column is = ");
      //Serial1.println(newColumn);
      //tempLeds[row][newColumn] = leds[row][column];
      //int newColumnFloor = floor(newColumn);
      //int newColumnCeil = ceil(newColumn);
      //int remainder = float((newColumn - floor(newColumn)) * 255.0);
    }
  }
}


