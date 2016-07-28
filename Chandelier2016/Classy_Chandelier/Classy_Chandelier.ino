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
#define serial Serial1      //if USB define as Serial, if Bluetooth define as Serial1 (hardware)
uint8_t numStrandStrip = numStrand / numStrip;

void runCommand(char);
void printKnob(float, float);
void runProgram();
void helpMenu();
CHSV twinkle_color( int );
void twinkle();
void sparkle();
float maxarr(float arr[]);
void computeVerticalLevels();
void transform(CRGB ledMatrix[numLedStrand][numStrand]);
void spin();
void printFFT();
void fadeleds();
void spectrum();
void pendulum();
void fireworks();
void fadeTempLeds();
void glitter();
void rainbowColumns();

uint8_t brightness = 128;
uint8_t maxBrightness = 255;
uint8_t increment = 8;
uint8_t maxKnob = 248;

uint8_t* currentKnob = &brightness;  //this is a knob pointer that points to the value I want to increment
void (*currentProgram)();            //this is a function pointer that points to the animation I want to run

CRGB currentColor = CRGB::Green ;

char mode = 't'; //start on twinkle mode

//for TwinkleSparkle
#define COOLING            3         // controls how quickly LEDs dim
#define TWINKLING          50        // controls how many new LEDs twinkle
#define FLICKER            50        // controls how "flickery" each individual LED is
unsigned int seeds =       0;
unsigned int sparklesEntered =      0;
static int heat[numLed];

//for spectrum
int spins = 0;
unsigned int theta = 0;
unsigned int spinSpeed = 5;          //inversely proportional to spin speed
float maxLevel = 0.0 ;
unsigned int freqBin, x ;
float peakLevelSpectrum = 0.0;

//for dimming
int levelForget = 1000;
float levels[numStrand] = {0};
float relativeLevels[numStrand] = {0};
float peakLevel = 0.0;

//for pendulum
int timescale = 100; // approximate voidloop steps per second
int offset = 200;
int t = 1;
int hue = 0;
int tchaos = 120 * timescale; // seconds to flip animation
int bias = 1;
int sinelevel = 0;
float relativeLevel = 0.0;
int timeConstants[numStrand] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2
};

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
  currentProgram = &twinkle; //start on twinkle
  AudioMemory(12);
  LEDS.addLeds<OCTOWS2811>(showLeds, numLedStrip).setCorrection( TypicalSMD5050 );
  LEDS.setBrightness(brightness);
  serial.begin(57600);
  pinMode(led, OUTPUT);
  //runCommand( 't' ); //start on twinkle
  for (int i = 0; i < numStrand; i++)
  {
    timeConstants[i] += offset;
  }

}

void loop() {
  while ( serial.available() > 0 ) {
    runCommand( serial.read() );
  }
  runProgram();
}

void runCommand(char command) {
  switch (command)
  {
    case '+':
    case '=':
      if (*currentKnob >= maxBrightness && currentKnob == &brightness ) {
        *currentKnob = maxBrightness ;
      }

      else if (*currentKnob >= maxKnob ) {
        *currentKnob = maxKnob ;
      }

      else {
        *currentKnob += increment ;
      }

      if (currentKnob == &brightness ) {
        printKnob(*currentKnob, maxBrightness);
      }
      else {
        printKnob(*currentKnob, maxKnob);
      }
      break;

    case '-':
    case '_':
      if (*currentKnob == 0) {
        *currentKnob = 0 ;
      }
      else {
        *currentKnob -= increment ;
      }

      if (currentKnob == &brightness ) {
        printKnob(*currentKnob, maxBrightness);
      }
      else {
        printKnob(*currentKnob, maxKnob);
      }
      break;

    case 'b':
      currentKnob = &brightness; // change where the pointer points
      serial.println("brightness");
      break;

    case 'h':
      helpMenu();
      //serial.println("Winker");
      break;

    //programs
    case 'T':
      currentProgram = &twinkle;
      serial.println("Twinkle");
      break;

    case 'X':
      currentProgram = &spectrum;
      serial.println("Spectrum");
      break;

    case 'P':
      currentProgram = &pendulum;
      serial.println("Pendulum");
      break;

    case 'F':
      for (int column = 0; column < numStrand; column++)
      {
        for (int row = 0; row < numLedStrand; row++)
        {
          leds[row][column] = CRGB::Black;
          //tempLeds[row][column] = CRGB::Black;
        }
      }
      currentProgram = &fireworks;
      serial.println("Fireworks");
      break;

    case 'G':
      currentProgram = &glitter;
      serial.println("Glitter");
      break;

    case 'R':
      currentProgram = &rainbowColumns;
      serial.println("Rainbow");
      break;

    //flourishes
    case '\'':
      sparklesEntered = 1; //this is 1 because you has to enter "s" to get here
      while (Serial.available() > 0) {
        Serial.read();
        sparklesEntered++;
      }
      sparkle();
      serial.println("sparkle");
      break;
  }
}

void runProgram()
{
  (*currentProgram)();
  LEDS.setBrightness(brightness);
  FastLED.show();
}

void printKnob(float knobNow, float knobMax)
{
  serial.print("value is ");
  serial.print( int( 100.0 * knobNow / knobMax )  );
  serial.println("%");
}

void helpMenu()
{
  serial.println("------------------------");
  serial.println("WELCOME TO THE HELP MENU");
  serial.println("------------------------");

  serial.println("PROGRAMS");
  serial.println(" T twinkle (first dance)");
  serial.println(" X spinny rainbow with stars");
  serial.println(" P pendulum with blue and pink");
  serial.println(" F fireworks");
  serial.println(" G glitter sound reactive");
  serial.println(" R rainbow");


  serial.println("FLOURISHES");
  serial.println(" ' sparkle (first dance)");

  serial.println("SETTINGS");
  serial.println(" h help");
  serial.println(" b brightness");
  serial.println(" + increase");
  serial.println(" - decrease");
  serial.println("------------------------");
}

CHSV twinkle_color( int temperature)
{
  CHSV heatcolor;
  heatcolor.hue = 40;
  heatcolor.saturation = 175;
  heatcolor.value = temperature;
  return heatcolor;
}

void twinkle()
{
  // Step 1. Create a randome number of seeds
  random16_add_entropy( random()); //random16() isn't very random, so this mixes things up a bit
  seeds = random16(10, numLed - 10); //the seeds change how many sites will get added heat

  // Step 2. "Cool" down every location on the strip a little
  for ( unsigned int i = 0; i < numLed; i++) {
    heat[i] = qsub8( heat[i], COOLING);
  }

  // Step 3. Make the seeds into heat on the string
  for ( unsigned int j = 0 ; j < seeds ; j++) {
    if (random16() < TWINKLING) {
      //again, we have to mix things up so the same locations don't always light up
      random16_add_entropy( random());
      heat[random16(numLed)] = random16(50, 255);
    }
  }

  // Step 4. Add some "flicker" to LEDs that are already lit
  //         Note: this is most visible in dim LEDs
  for ( unsigned int k = 0 ; k < numLed ; k++ ) {
    if (heat[k] > 0 && random8() < FLICKER) {
      heat[k] = qadd8(heat[k] , 10);
    }
  }

  // Step 5. Map from heat cells to LED colors
  for ( unsigned int j = 0; j < numLed; j++) {
    showLeds[j] = twinkle_color( heat[j] );
  }
}

void sparkle() {
  for (unsigned int sparkles = 0 ; sparkles < sparklesEntered ; sparkles ++) {
    seeds = random16(numLed / 8, numLed / 2);
    for ( unsigned int i = 0 ; i < seeds ; i++) {
      unsigned int pos = random16(numLed);
      random16_add_entropy( random() );
      heat[pos] = qadd8(heat[pos] , random8(50, 150));
    }
    for ( unsigned int j = 0; j < numLed; j++) {
      showLeds[j] = twinkle_color( heat[j] );
    }
    FastLED.show();
  }
}

void spectrum() {
  for ( theta = 0 ; theta < numStrand * spinSpeed ; theta ++)
  {
    unsigned int freqBin, x ;
    //float level;

    if (fft.available())
    {
      freqBin = 2; //ignore the first two bins which contain DC offsets
      for (x = 0; x < numStrand; x++) {
        levels[x] = fft.read(freqBin, freqBin + frequencyBinsHorizontal[x] - 1);
        freqBin = freqBin + frequencyBinsHorizontal[x];
      }
    }

    maxLevel = maxarr(levels) ;
    if ( maxLevel > peakLevelSpectrum)
    {
      peakLevelSpectrum = maxLevel;
    }

    Serial.print("max level = ");
    Serial.println(maxLevel);
    Serial.print("peak level = ");
    Serial.println(peakLevelSpectrum);

    //relativeLevels = levels / peakLevelSpectrum ;
    for (x = 0; x < numStrand; x++) {
      relativeLevels[x] = levels[x] / peakLevelSpectrum ;
    }
    peakLevelSpectrum = peakLevelSpectrum - peakLevelSpectrum / levelForget;

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
          if (random8(maxLevel / peakLevelSpectrum * 255, 255) > 253  && random8(maxLevel / peakLevelSpectrum * 255, 255) > 253 && random8() > 250)
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
  for (unsigned int i = 0 ; i < sizeof(arr) ; i++)
  {
    if (arr[i] > maxVal)
    {
      maxVal = arr[i] ;
    }
    else {}
  }
  return maxVal ;
}

void pendulum() {
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

void fadeTempLeds()
{
  for (int row = 0; row < numLedStrand; row++) {
    for (int column = 0; column < numStrand; column++) {
      tempLeds[row][column].nscale8(35);
    }
  }
}

void fireworks() {
  for (int column = 0; column < numStrand; column++)
  {
    for (int row = 0; row < numLedStrand; row++)
    {
      if (random8() > 250 && random8() > 250)
      {
        leds[row][column] = CHSV{random(0, 255), random8(), 255};
      }

      if (row > 1 && row < numLedStrand - 2 && column > 0 && column < numStrand - 1 )
      {
        if (column == 0) {
          tempLeds[row - 1][numStrand - 1] += leds[row][column];
          tempLeds[row + 1][numStrand - 1] += leds[row][column];
          tempLeds[row - 1][column + 1] += leds[row][column];
          tempLeds[row + 1][column + 1] += leds[row][column];
          tempLeds[row - 2][numStrand - 1] += leds[row][column];
          tempLeds[row + 2][numStrand - 1] += leds[row][column];
          tempLeds[row - 2][column + 1] += leds[row][column];
          tempLeds[row + 2][column + 1] += leds[row][column];
        }

        else if (column == numStrand - 1) {
          tempLeds[row - 1][column - 1] += leds[row][column];
          tempLeds[row + 1][column - 1] += leds[row][column];
          tempLeds[row - 1][0] += leds[row][column];
          tempLeds[row + 1][0] += leds[row][column];
          tempLeds[row - 2][column - 1] += leds[row][column];
          tempLeds[row + 2][column - 1] += leds[row][column];
          tempLeds[row - 2][0] += leds[row][column];
          tempLeds[row + 2][0] += leds[row][column];
        }

        else {
          tempLeds[row - 1][column - 1] += leds[row][column];
          tempLeds[row + 1][column - 1] += leds[row][column];
          tempLeds[row - 1][column + 1] += leds[row][column];
          tempLeds[row + 1][column + 1] += leds[row][column];
          tempLeds[row - 2][column - 1] += leds[row][column];
          tempLeds[row + 2][column - 1] += leds[row][column];
          tempLeds[row - 2][column + 1] += leds[row][column];
          tempLeds[row + 2][column + 1] += leds[row][column];
        }
        tempLeds[row][column] += leds[row][column];
      }
    }
  }
  fadeTempLeds();
  for (int column = 0; column < numStrand; column++)
  {
    for (int row = 0; row < numLedStrand; row++)
    {
      leds[row][column] = tempLeds[row][column] ;
      tempLeds[row][column] = CRGB::Black;
    }
  }
  transform(leds);
  fadeleds();
  delay(50);
  //FastLED.show();
}

void glitter() {
  unsigned int freqBin, x, y ;
  float level = 0;
  float bassLevel = 0;
  float midLevel = 0;
  float trebLevel = 0;

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
    peakLevel = peakLevel - peakLevel / levelForget;
    relativeLevel = level / peakLevel;

    for (int column = 0; column < numStrand; column++)
    {
      for (int row = 0; row < numLedStrand; row++)
      {
        if (relativeLevel * 255 * (row) / numLedStrand > random8() + random8())
        {
          leds[row][column] += CHSV(40, 150, random8(0, 255)) ;
        }
      }
    }
    transform(leds);
    fadeleds();
    FastLED.show();
  }
}

void rainbowColumns()
{
  for (int hue = 255 ; hue >=  0 ; hue --) {
    for (int column = 0; column < numStrand; column++)
    {
      CRGB rainLeds[numLedStrand];
      fill_rainbow (rainLeds, numLedStrand, hue, 2 );
      for ( int row = 0; row < numLedStrand; row++ )
      {
        leds[row][column] = rainLeds[row];
      }
    }
    transform(leds);
    delay(20);
    FastLED.show();
  }
}

