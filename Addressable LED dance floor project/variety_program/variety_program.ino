//Some light chase hacks for the AM-2640 5V, Addressable LED strips http://www.andymark.com/product-p/am-2640.htm based on the WS2801 chipset
//We ran this demo off of our AM-2287 Arduino Ethernet http://www.andymark.com/product-p/am-2287.htm
//http://arduino.cc/en/Main/ArduinoBoardEthernet
//The FastLED library we use here supports multiple chipsets
//This code requires that the fastspi library be put in your arduino\libraries folder
//Arduino info on how to install software libraries http://arduino.cc/en/Guide/Libraries
//AndyMark, Inc.
//Craig Kessler 12/3/2013, 3/17/2014, 3/20/2014


//***NOTE: This strip runs off of 5V MAX!!!. Applying much more than 5V will damage/destroy you LED strip!***
//***Handling note: Don't mess with the wiring while the power is on. This can cause voltage spikes ***
//***or sneak ground paths that can damage the LED strip ***

//DO NOT try to power the whole strip (80 LEDs) off the arduino 5v regulator.
//At full bright white, the strip can draw 1.5Amps or so. This will overheat or burnout the regulator.
//Remember on the AM-2640 LED strip's yellow wire is ground! (don't blame us they come that way)
//Make sure you connect the Yellow ground from the LED strip to the Arduino ground.
//Communications to the LEDs requires a common ground to work.

//We recommend running these led strips off of the AM-0899 10-30Vin to 5V 5A out stepdown converter http://www.andymark.com/product-p/am-0899.htm

//If you are using the AndyMark AM-2297 Arduino Ethernet board then make sure you select Tools>Board>Arduino Ethernet from the Arduino IDE menu
//If you are new to working with Arduino a good place to start is here http://arduino.cc/en/Guide/HomePage
//Another new training resource provided by a 3rd party is here: http://www.arduinoclassroom.com/index.php/arduino-101



//CSK 3/17/2013 Libraries new location
//https://github.com/FastLED/FastLED
//https://github.com/FastLED/FastLED/wiki/Overview

//#include "FastSPI_LED2.h"
//CSK 3/17/2014 FastSPI library has been updated.  The new header file name is just FastLED.h. FastSPI_LED2.h is now just a shell with an include for FastLED.h
#include "FastLED.h"

//Tell it how many leds are in the strip. AndyMark's 2.5 meter strip has 80 leds
#define NUM_LEDS 60
// This is an array of leds. One item for each led in your strip.
CRGB leds[NUM_LEDS];

//CSK 3/17/2014 I moved these to pins that don't conflict with Ethernet functions in case you want to control LEDs via Ethernet
#define DATA_PIN 3 //Green wire from AM-2640's power connector
// Clock pin SPI
#define CLOCK_PIN 4 //Blue wire from AM-2640's power connector

#define MAX_BRIGHTNESS 255

#define NUM_LEDS           60
#define DATA_PIN           3
#define COLOR_ORDER        GRB
#define CHIPSET            NEOPIXEL
#define BRIGHTNESS         100
#define FRAMES_PER_SECOND  500

//This function is used to setup things like pins, Serial ports etc.
//Here we specify which chipset our LEDs run off of by our choice of config function
void setup()
{
  // Uncomment one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
  //FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);

  delay(2000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  //LEDS.setBrightness(BRIGHTNESS);

  //FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);

  //***This is the chipset in the AM-2640 LED strip***
  //CSK 3/17/2013 Changed to this function to allow direct data and clock pin specification
  //FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
}

void loop()
{
  //This is kind of Arduino's equivalent to Main() in a standard C program
  //This, as the name implies, loops endlessly.
  //https://code.google.com/p/fastspi/wiki/CRGBreference
  //CSK 3/20/2014 I added a rainbow function just for grins
  rainbow(20);
  cylon(CRGB::Red,25, 5);
  color_chase(CRGB::Green, 15);
  color_chase(CRGB::BlueViolet, 15);
  color_chase(CRGB::Red, 15);
  color_chase(CRGB::Yellow, 15);
  color_chase(CRGB::Green, 15);
  color_chase(CRGB::Cyan, 15);
  color_chase(CRGB::Blue, 15);
  missing_dot_chase(CRGB::White, 20);
  missing_dot_chase(CRGB::Red, 20);
  missing_dot_chase(CRGB::Yellow, 20);
  missing_dot_chase(CRGB::Green, 20);
  missing_dot_chase(CRGB::Cyan, 20);
  missing_dot_chase(CRGB::Blue, 20);
  missing_dot_chase(0x3000cc, 20) ;
}

//These are the functions we have defined to do chase patterns.  They are actually called inside the  loop() above
//They are meant to demonstrate things such as setting LED colors, controlling brightness
void color_chase(uint32_t color, uint8_t wait)
{
  //clear() turns all LEDs off
  FastLED.clear();
  //The brightness ranges from 0-255
  //Sets brightness for all LEDS at once
  FastLED.setBrightness(MAX_BRIGHTNESS);
  // Move a single led
  for(int led_number = 0; led_number < NUM_LEDS; led_number++)
  {
    // Turn our current led ON, then show the leds
    leds[led_number] = color;

    // Show the leds (only one of which is has a color set, from above
    // Show turns actually turns on the LEDs
    FastLED.show();

    // Wait a little bit
    delay(wait);

    // Turn our current led back to black for the next loop around
    leds[led_number] = CRGB::Black;
  }
  return;
}

//Move an "empty" dot down the strip
void missing_dot_chase(uint32_t color, uint8_t wait)
{
  //Step thru some brightness levels from max to 10.  led_brightness/=2 is a cryptic shorthand way of saying led_brightness = led_brightness/2
  for (int led_brightness = MAX_BRIGHTNESS; led_brightness > 10; led_brightness/=2)
  {
    FastLED.setBrightness(led_brightness);

    // Start by turning all pixels on:
    //for(int led_number = 0; led_number < NUM_LEDS; led_number++) leds[led_number] = color;
    //https://github.com/FastLED/FastLED/wiki/Controlling-leds
    fill_solid(leds, NUM_LEDS, color);

    // Then display one pixel at a time:
    for(int led_number = 0; led_number < NUM_LEDS; led_number++)
    {
      leds[led_number] =  CRGB::Black; // Set new pixel 'off'
      if( led_number > 0 && led_number < NUM_LEDS)
      {
        leds[led_number-1] = color; // Set previous pixel 'on'
      }
      FastLED.show();
      delay(wait);
    }
  }
  return;
}

//Cylon - LED sweeps back and forth, with the color, delay and number of cycles of your choice 
void cylon(CRGB color, uint16_t wait, uint8_t number_of_cycles)
{
  FastLED.setBrightness(255);
  for (uint8_t times = 0; times<=number_of_cycles; times++)
  {
    // Make it look like one LED is moving in one direction
    for(int led_number = 0; led_number < NUM_LEDS; led_number++)
    {
      //Apply the color that was passed into the function
      leds[led_number] = color;
      //Actually turn on the LED we just set
      FastLED.show();
      // Turn it back off
      leds[led_number] = CRGB::Black;
      // Pause before "going" to next LED
      delay(wait);
    }

    // Now "move" the LED the other direction
    for(int led_number = NUM_LEDS-1; led_number >= 0; led_number--)
    {
      //Apply the color that was passed into the function
      leds[led_number] = color;
      //Actually turn on the LED we just set
      FastLED.show();
      // Turn it back off
      leds[led_number] = CRGB::Black;
      // Pause before "going" to next LED
      delay(wait);
    }
  }
  return;
}

void rainbow(uint8_t wait) 
{

  uint16_t hue;
  FastLED.clear();

  for(hue=10; hue<255*3; hue++) 
  {

    fill_rainbow( &(leds[0]), NUM_LEDS /*led count*/, hue /*starting hue*/);		
    FastLED.show();
    delay(wait);
  }
  return;
}

