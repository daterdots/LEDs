

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

float maxarr(float arr[]);
void computeVerticalLevels();
void transform();
void transformTemp();
void black();
void spin();
void printFFT();
void fadeleds();



// The display size and color to use
const unsigned int matrix_width = 32;
const unsigned int matrix_height = 75;
const unsigned int myColor = 0x400020;

void transform(CRGB ledMatrix[matrix_height][matrix_width]);

#define BRIGHTNESS  255
#define numStrip 8          //strips are contiguous physical LED chains
#define numStrand 32        //strands are the vertical "strands" that make up the chandelier
#define numLedStrip 300
#define numLedStrand 75
#define numLed 2400
uint8_t globalBrightness = 128;
uint8_t numStrandStrip = 4;

int levelForget = 1000;
float levels[numStrand] = {0};
float relativeLevels[numStrand] = {0};
float peakLevel = 0.0;
float maxLevel = 0.0 ;

int spins = 0;
int theta = 0;
unsigned int spinSpeed = 5;//inversely proportional to spin speed

CRGB leds[numLedStrand][numStrand];
CRGB tempLeds[numLedStrand][numStrand];
CRGB showLeds[numLed];
CRGB SparklerColor(int temperature);

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
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  2,
  2,
  3,
  3,
  4,
  5,
  5,
  6,
  7,
  8,
  10,
  11,
  13,
  15,
  17,
  20,
  23,
  26,
  30,
  35,
  40,
  46,
  53,
  61,
  70

  //  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
  //  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,
  //  6,  6,  6,  6,  8,  8,  8,  8,  8,  10,
  //  10, 10
};

// Run setup once
void setup() {
  // the audio library needs to be given memory to start working
  AudioMemory(12);

  LEDS.addLeds<OCTOWS2811>(showLeds, numLedStrip);
  LEDS.setBrightness(globalBrightness);
}

void loop() {
  for ( theta = 0 ; theta < numStrand * spinSpeed ; theta ++)
  {
    unsigned int freqBin, x, y ;
    float level;

    if (fft.available())
    {
      freqBin = 2; //ignore the first two bins which contain DC offsets
      for (x = 0; x < matrix_width; x++) {
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
    for (x = 0; x < matrix_width; x++) {
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
          if (random8(maxLevel/peakLevel * 255, 255) > 253  && random8(maxLevel/peakLevel * 255, 255) > 253 && random8()>250)
          {
            leds[numLedStrand - row - 1][column] += CHSV(0,0,random8(150,255)) ;
            leds[numLedStrand - row - 1][numStrand - 1 - column] += CHSV(0,0,random8(150,255)) ;
          }
          else{}
        }
      }
    }


    spin();
    transform(tempLeds);
    FastLED.show();
    fadeleds();
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

void black()
{
  for (int i = 0 ; i < numLed ; i++)
  {
    showLeds[i] = CRGB::Black;
  }
}

void printFFT() {
  for (int i = 0; i < matrix_width; i++) {
    if (relativeLevels[i] > 0.1)
    {
      Serial.print(" ");
      Serial.print(relativeLevels[i]);
      Serial.print(" ");
    }

    else
    {
      Serial.print(" ---- ");
    }
  }
  Serial.println();
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

float maxarr(float arr[])
{
  float maxVal = 0 ;
  for (int i = 0 ; i < sizeof(arr) ; i++)
  {
    if (arr[i] > maxVal)
    {
      maxVal = arr[i] ;
    }
    else {}
  }
  return maxVal ;
}

