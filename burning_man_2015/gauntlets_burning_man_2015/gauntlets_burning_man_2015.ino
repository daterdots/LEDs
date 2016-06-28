////////////////////////////////////////////////////////////////
//  burning_man_2015_master is a program that lets you
//  cycle through animations using a V Keyes encoder
//
//  Daniel Wilson, 2015
//
//  With BIG thanks to the FastLED community!
////////////////////////////////////////////////////////////////

//Add libraries and set global constants
#include "FastLED.h"
#define NUM_LEDS      120
#define DATA_PIN      3
#define COLOR_ORDER   GRB
#define CHIPSET       NEOPIXEL
CRGB leds[NUM_LEDS];
static byte heat[NUM_LEDS];
static uint8_t brightness = 128;    // without STATIC it does not count correctly!!!
static uint8_t programCounter = 0;
static uint8_t program = 0;

//Add variables for each of the sub programs
//Programs are named before the variables are defined
//Bouncing Balls
#define GRAVITY           -10                // Downward (negative) acceleration of gravity in m/s^2
#define h0                1                  // Starting height, in meters, of the ball (strip length)
#define NUM_BALLS         6                  // Number of bouncing balls you want (recommend < 7, but 20 is fun in its own way)
float h[NUM_BALLS] ;                         // An array of heights
float vImpact0 = sqrt( -2 * GRAVITY * h0 );  // Impact velocity of the ball when it hits the ground if "dropped" from the top of the strip
float vImpact[NUM_BALLS] ;                   // As time goes on the impact velocity will change, so make an array to store those values
float tCycle[NUM_BALLS] ;                    // The time since the last time the ball struck the ground
int   pos[NUM_BALLS] ;                       // The integer position of the dot on the strip (LED index)
long  tLast[NUM_BALLS] ;                     // The clock time of the last ground strike
float COR[NUM_BALLS] ;                       // Coefficient of Restitution (bounce damping)

//Fire2012
#define FIRE_COOLING  70
#define FIRE_SPARKING 100

//America
#define SPARKLER_SPARKING           150
#define SPARKLER_CHILL_SPARKING     10
#define SPARKLER_COOLING            100

//Strober
unsigned int seeds = 0;
uint8_t mic_mean = 280;
#define TWINKLE_COOLING            20         // controls how quickly LEDs dim
#define TWINKLE_TWINKLING          100       // controls how many new LEDs twinkle
#define TWINKLE_FLICKER            50        // controls how "flickery" each individual LED is

//For audio
int sensor = 0;
int sensorValue = 0;
int runningSensor = 0;
uint8_t levelOld = 0;
uint8_t color = 0;
uint8_t level = 0;
int samples = 20 ;
long rmsValue = 0;

//happyAccident
uint8_t count = 0; //count is an unsigned 8 bit integer limited to values between 0 and 255
uint8_t walker = 0;
#define low 20
#define high  150
#define delay_time 20

//for Keyes encoder
volatile boolean TurnDetected;
volatile boolean up;
#define PinCLK 2                   // Used for generating interrupts using CLK signal
#define PinDT 5                    // Used for reading DT signal
#define PinSW 6                    // Used for the push button switch
#define brightnessIncrement 4      // Amount to increment the brightness per knob turn
#define NUM_PROGRAMS 11
void isr ()  {                     // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK of the V Keyes
  if (digitalRead(PinCLK))
    up = digitalRead(PinDT);
  else
    up = !digitalRead(PinDT);
  TurnDetected = true;
}

void setup ()  {
  delay(2000);                               // sanity check delay - allows reprogramming if accidently blowing power w/leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(PinCLK, INPUT);
  pinMode(PinDT, INPUT);
  pinMode(PinSW, INPUT);
  attachInterrupt (1, isr, FALLING); // interrupt 1 is always connected to pin 2 on Arduino Micro
  //Serial.begin (9600);
  //Serial.println("Start");
}

// Loop is the main function in this program
// It checks to see if the user has incremented the program or the LED brightness
void loop ()  {
  if (!(digitalRead(PinSW))) {                // check if pushbutton is pressed on the V Keyes
    delay(500);
    programCounter++;                         // increment the program counter
    program = programCounter % NUM_PROGRAMS;  // the program to choose is the modulo of the program counter
  }

  if (TurnDetected)  {                        // only change brightness if a rotation was detected
    if (up)
      brightness = brightness + brightnessIncrement;
    else
      brightness = brightness - brightnessIncrement;
    TurnDetected = false;                     // do NOT repeat IF loop until new rotation detected
    LEDS.setBrightness(brightness);
  }
  checkProgram(); 
}

void checkProgram() {
  switch (program)
  {
    case 0:
      //Serial.print("Program is: ");
      //Serial.println(program);
      cylon();
      break;

    case 1:
      //Serial.print("Program is: ");
      //Serial.println(program);
      stochasticChill();
      break;

    case 2:
      //Serial.print("Program is: ");
      //Serial.println(program);
      happyAccident();
      break;

    case 3:
      //Serial.print("Program is: ");
      //Serial.println(program);
      rainbow();
      break;

    case 4:
      //Serial.print("Program is: ");
      //Serial.println(program);
      blinker();
      break;

    case 5:
      //Serial.print("Program is: ");
      //Serial.println(program);
      Fire2012();
      break;

    case 6:
      //Serial.print("Program is: ");
      //Serial.println(program);
      fastCylon();
      break;

    case 7:
      //Serial.print("Program is: ");
      //Serial.println(program);
      for (int i = 0 ; i < NUM_BALLS ; i++) {    // Initialize variables
        tLast[i] = millis();
        h[i] = h0;
        pos[i] = 0;                              // Balls start on the ground
        vImpact[i] = vImpact0;                   // And "pop" up at vImpact0
        tCycle[i] = 0;
        COR[i] = 0.90 - float(i) / pow(NUM_BALLS, 2);
      }
      bouncingBalls();
      break;

    case 8:
      //Serial.print("Program is: ");
      //Serial.println(program);
      america();
      break;

    case 9:
      //Serial.print("Program is: ");
      //Serial.println(program);
      strober();
      break;

    case 10 :
      //Serial.print("Program is: ");
      //Serial.println(program);
      soundRainbow();
      break;
  }
}

void cylon() {
  // First slide the led in one direction
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    leds[i] = CHSV(i * 2 + count, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    delay(50);
  }

  // Now go in the other direction.
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    // Set the i'th led to red
    leds[i] = CHSV(i * 2 + count, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    count++;
    delay(50);
  }
}

void fastCylon() {
  // First slide the led in one direction
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    leds[i] = CHSV(i * 2 + count, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    delay(2);
  }

  // Now go in the other direction.
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    // Set the i'th led to red
    leds[i] = CHSV(i * 2 + count, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    count++;
    delay(2);
  }
}

void blinker() {
  for (int i = 0; i < 255; i++) {
    fill_solid(leds, NUM_LEDS, CHSV(i, 255, 255));
    FastLED.show();
    delay(200);
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    // Now turn the LED off, then pause
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(200);
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
  }
  //return;
}

void bouncingBalls() {
  while (!TurnDetected && digitalRead(PinSW)) {
    for (int i = 0 ; i < NUM_BALLS ; i++) {
      tCycle[i] =  millis() - tLast[i] ;     // Calculate the time since the last time the ball was on the ground
      // A little kinematics equation calculates positon as a function of time, acceleration (gravity) and intial velocity
      h[i] = 0.5 * GRAVITY * pow( tCycle[i] / 1000 , 2.0 ) + vImpact[i] * tCycle[i] / 1000;

      if ( h[i] < 0 ) {
        h[i] = 0;                            // If the ball crossed the threshold of the "ground," put it back on the ground
        vImpact[i] = COR[i] * vImpact[i] ;   // and recalculate its new upward velocity as it's old velocity * COR
        tLast[i] = millis();
        if ( vImpact[i] < 0.01 ) vImpact[i] = vImpact0;  // If the ball is barely moving, "pop" it back up at vImpact0
      }
      pos[i] = round( h[i] * (NUM_LEDS - 1) / h0);       // Map "h" to a "pos" integer index position on the LED strip
    }

    //Choose color of LEDs, then the "pos" LED on
    for (int i = 0 ; i < NUM_BALLS ; i++) leds[pos[i]] = CHSV( uint8_t (i * 40) , 255, 255);
    FastLED.show();
    //Then off for the next loop around
    for (int i = 0 ; i < NUM_BALLS ; i++) {
      leds[pos[i]] = CRGB::Black;
    }
  }
}

void Fire2012() //credit Mark Kriegsman https://blog.kriegsman.org/
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((FIRE_COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < FIRE_SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    leds[j] = HeatColor( heat[j]);
  }
  delay(30);
  FastLED.show();
}

void america()
{
  static byte heat[NUM_LEDS];

  random16_add_entropy( random());
  FastLED.show(); // display this frame
  delay(20);

  //Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i], SPARKLER_COOLING / 255 + 1);
  }

  // Randomly ignite new 'sparks'
  if ( random8() < SPARKLER_SPARKING ) {
    random16_add_entropy( random());
    int y = random8(NUM_LEDS); //y is the position, make sparks in this range
    heat[y] = qadd8( heat[y], random8(150, 255) ) - 1 ; //the -1 is here because of the 255 == yellow bug
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++)
  {
    leds[j] = SparklerColor( heat[j]);
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
    heatcolor.b = heatramp * 30 / 255; // no blue

  }
  else {
    // we're in the coolest third
    heatcolor.r = heatramp * 40 / 255; // ramp up red
    heatcolor.g = heatramp * 130 / 255; // no green
    heatcolor.b = heatramp; // no blue
  }
  return heatcolor;
}


void strober()
{
  // Step 1. Create a randome number of seeds
  random16_add_entropy( random()); //random8() isn't very random, so this mixes things up a bit
  seeds = random8(10, NUM_LEDS - 10);

  // Step 2. "Cool" down every location on the strip a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i], TWINKLE_COOLING);
  }

  // Step 3. Make the seeds into heat on the string
  for ( int j = 0 ; j < seeds ; j++) {
    if (abs(analogRead(A5) - 265) > TWINKLE_TWINKLING) {
      //again, we have to mix things up so the same locations don't always light up
      random16_add_entropy( random());
      heat[random8(NUM_LEDS)] = random8(50, 255);
    }
  }

  // Step 4. Add some "flicker" to LEDs that are already lit
  //         Note: this is most visible in dim LEDs
  for ( int k = 0 ; k < NUM_LEDS ; k++ ) {
    if (heat[k] > 0 && random8() < TWINKLE_FLICKER) {
      heat[k] = qadd8(heat[k] , 10);
    }
  }

  // Step 5. Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++)
  {
    leds[j] = TwinkleColor( heat[j] );
  }
  //nextTwinkle += 1000 / FRAMES_PER_SECOND ; // assign the next time Twinkle() should happen
  FastLED.show();
}

//Play with this for different strip colors
CHSV TwinkleColor( int temperature)
{
  CHSV heatcolor;
  heatcolor.hue = 60;
  heatcolor.saturation = 0;
  heatcolor.value = temperature;
  return heatcolor;
}

void soundRainbow()
{
  // Turn the LED on, then pause
  //Serial.println("in sound rainbow");
  rmsValue = 0;
  for (int i = 0; i < samples; i++) {
    sensorValue = analogRead(A5);    // read analog input to sensorValue
    rmsValue = rmsValue + sq( (float)sensorValue );
  }
  //Serial.print("pre-squared value is: ");
  //Serial.println(rmsValue);
  rmsValue = sqrt(rmsValue / (samples) );
  //Serial.print("post-squared value is: ");
  //Serial.println(rmsValue);
  level = (2 * abs(rmsValue - 246) + levelOld) / 2;
  //Serial.println(level);

  if (level < 25) {
    level = 25;
  }

  for ( int j = 0; j < NUM_LEDS; j++)
  {
    leds[j] = CHSV(color + j, 255, level);
  }
  color++;
  levelOld = level /  2 + levelOld / 2;
  FastLED.show();
}

void stochasticChill()
{
  static byte heat[NUM_LEDS];

  random16_add_entropy( random());
  FastLED.show(); // display this frame
  delay(20);

  //Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i], SPARKLER_COOLING / 255 + 1);
  }

  // Randomly ignite new 'sparks'
  if ( random8() < SPARKLER_CHILL_SPARKING ) {
    random16_add_entropy( random());
    int y = random8(NUM_LEDS); //y is the position, make sparks in this range
    heat[y] = qadd8( heat[y], random8(150, 255) ) - 1 ; //the -1 is here because of the 255 == yellow bug
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS; j++)
  {
    leds[j] = SparklerChillColor( heat[j]);
  }
}

CRGB SparklerChillColor(int temperature)
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
    heatcolor.g = heatramp * 60 / 255; // ramp up green
    heatcolor.b = heatramp * 50 / 255; // no blue

  }
  else {
    // we're in the coolest third
    heatcolor.r = heatramp * 40 / 255; // ramp up red
    heatcolor.g = heatramp * 80 / 255; // no green
    heatcolor.b = heatramp; // no blue
  }
  return heatcolor;
}

void rainbow()
{
  // Turn the LED on, then pause
  //Serial.println("in sound rainbow");
  for ( int j = 0; j < NUM_LEDS; j++)
  {
    leds[j] = CHSV(color + j, 180, 255);
  }
  color++;
  delay(1);
  FastLED.show();
}

void happyAccident() {
  // First, set the colors of all the LEDs on the strip
  for (int j = low; j < high; j++) {
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    for (int i = 0; i < NUM_LEDS / 2; i++) {
      // Set the i'th led a color defined by a multiple of i plus 'count'
      walker = i + count;
      if (walker % 8 == 1) {
        leds[i] = CHSV(count + i * 8, 190, j);
        //Serial.println(j);
      }
      else {
        leds[i] = CRGB::Black;
      }
    }

    //for (int i = 0; i < NUM_LEDS / 2; i++) {
    //  storage2[i] = leds[NUM_LEDS / 2 - i - 1];
    //}

    for (int i = 0; i < NUM_LEDS / 2; i++) {
      leds[NUM_LEDS - i] = leds[i];
    }
    count++; //index count up
    // Show the leds
    FastLED.show();
    // Wait a little bit before we loop around and do it again
    delay(delay_time);
  }

  ////////////
  for (int j = high; j > low; j--) {
    if (TurnDetected || !(digitalRead(PinSW)))  {        // do this only if rotation or push was detected
      return;
    }
    for (int i = 0; i < NUM_LEDS / 2; i++) {
      // Set the i'th led a color defined by a multiple of i plus 'count'
      walker = i + count;
      if (walker % 8 == 1) {
        leds[i] = CHSV(count + i * 8, 190, j);
        //Serial.println(j);
      }
      else {
        leds[i] = CRGB::Black;
      }
    }

    //for (int i = 0; i < NUM_LEDS / 2; i++) {
    //  storage2[i] = leds[NUM_LEDS / 2 - i - 1];
    //}

    for (int i = 0; i < NUM_LEDS / 2; i++) {
      leds[NUM_LEDS - i] = leds[i];
    }
    count++; //index count up
    // Show the leds
    FastLED.show();
    // Wait a little bit before we loop around and do it again
    delay(delay_time);
  }
}
