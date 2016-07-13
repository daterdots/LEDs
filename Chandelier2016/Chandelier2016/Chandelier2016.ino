void transform();
void columnsAndRows();
void rainbowColumns();
void america();
void spiral();
void hula();

//America
#define SPARKLER_SPARKING           240
#define SPARKLER_CHILL_SPARKING     10
#define SPARKLER_COOLING            200

#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

#define numStrip 8          //strips are contiguous physical LED chains
#define numStrand 32        //strands are the vertical "strands" that make up the chandelier
#define numLedStrip 300
#define numLedStrand 75
#define numLed 2400

uint8_t numStrandStrip = 4;
uint8_t globalBrightness = 255;
#define led 13
char mode = 'a';

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


  //columnsAndRows();
  rainbowColumns();
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
      transform();
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
      transform();
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
    transform();
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
    transform();
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
    transform();
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
    transform();
    FastLED.show();
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

void checkProgram() {
  switch (program)
  {
    case 0:
      cylon();
      break;

    case 1:
      stochasticChill();
      break;

  }
}
