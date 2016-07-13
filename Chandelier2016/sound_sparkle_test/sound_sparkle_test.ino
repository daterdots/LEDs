
void computeVerticalLevels();
void transform();
void transformTemp();
void black();
void spin();

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

// The display size and color to use
const unsigned int matrix_width = 32;
const unsigned int matrix_height = 75;
const unsigned int myColor = 0x400020;


#define BRIGHTNESS  255
#define numStrip 8          //strips are contiguous physical LED chains
#define numStrand 32        //strands are the vertical "strands" that make up the chandelier
#define numLedStrip 300
#define numLedStrand 75
#define numLed 2400
uint8_t globalBrightness = 128;
uint8_t numStrandStrip = 4;

int spins = 0;
int theta = 0;
unsigned int spinSpeed = 200000;//inversely proportional to spin speed
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
  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
  6,  6,  6,  6,  8,  8,  8,  8,  8,  10,
  10, 10
};
//,  6,  6,  6,  7,  7,  7,  8,  8,
//9,  9, 10, 10, 11, 12, 12, 13, 14, 15,
//15, 16, 17, 18, 19, 20, 22, 23, 24, 25
//};

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
  Serial.begin(9600);
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
      leds[row][column].nscale8(240);
    }
  }
}

void loop() {
  unsigned int freqBin, x, y ;
  float level = 0;
  float bassLevel = 0;
  float midLevel = 0;
  float trebLevel = 0;

  if (fft.available()) {
    freqBin = 5;
    for (int freq = 0 ; freq < numStrand ; freq++ )
    {
      level = level + fft.read(freqBin, freqBin + frequencyBinsHorizontal[freq] - 1);

      if (freq < 10)
      {
        bassLevel = bassLevel + fft.read(freqBin, freqBin + frequencyBinsHorizontal[freq] - 1);
      }

      else if (freq < 20)
      {
        midLevel = midLevel + fft.read(freqBin, freqBin + frequencyBinsHorizontal[freq] - 1);
      }

      else
      {
        trebLevel = trebLevel + fft.read(freqBin, freqBin + frequencyBinsHorizontal[freq] - 1);
      }
    }


    ////// level
    if (level > peakLevel)
    {
      peakLevel = level;
    }
    peakLevel = peakLevel - peakLevel / levelForget;
    relativeLevel = level / peakLevel;

    ////// bass
    if (bassLevel > peakBassLevel)
    {
      peakBassLevel = bassLevel;
    }
    peakBassLevel = peakBassLevel - peakBassLevel / levelForget;
    relativeBassLevel = bassLevel / peakBassLevel;

    ////// mid
    if (midLevel > peakMidLevel)
    {
      peakMidLevel = midLevel;
    }
    peakMidLevel = peakMidLevel - peakMidLevel / levelForget;
    relativeMidLevel = midLevel / peakMidLevel;

    ////// treb
    if (trebLevel > peakTrebLevel)
    {
      peakTrebLevel = trebLevel;
    }
    peakTrebLevel = peakTrebLevel - peakTrebLevel / levelForget;
    relativeTrebLevel = trebLevel / peakTrebLevel;



    //Serial.print("level is ");
    //Serial.println(level);

    //    for (int column = 0; column < numStrand; column++)
    //    {
    //      for (int row = 0; row < numLedStrand; row++)
    //      {
    //        if (level * numLedStrand * 10 > row + 1)
    //        {
    //          leds[row][column] = CRGB::White ;
    //        }
    //      }
    //    }

    for (int column = 0; column < numStrand; column++)
    {
      for (int row = 0; row < numLedStrand; row++)
      {
        if (relativeLevel * 255 > random8() + random8())
        {
          leds[row][column] += CHSV(0, 0, random8(255)) ;
        }
        //        if (relativeBassLevel * 255 > random8() + random8())
        //        {
        //          leds[row][column] += CHSV(0, 255, random8(255)) ;
        //        }
        //        if (relativeMidLevel * 255 > random8() + random8())
        //        {
        //          leds[row][column] += CHSV(80, 255, random8(255)) ;
        //        }
        //        if (relativeTrebLevel * 255 > random8() + random8())
        //        {
        //          leds[row][column] += CHSV(160, 255, random8(255)) ;
        //        }
      }
    }

    transform();
    fadeleds();
    FastLED.show();
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

void transform()
{
  for (int column = 0; column < numStrand; column++)
  {
    int strip = floor(column / numStrandStrip);
    for (int row = 0; row < numLedStrand; row++)
    {
      if ((column - strip * numStrandStrip) % 2 == 1) //check to see if this is the strip that goes bottom to top
      {
        showLeds[column * numLedStrand + row] = leds[numLedStrand - row - 1][column];
      }
      else
      {
        showLeds[column * numLedStrand + row] = leds[row][column];
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


