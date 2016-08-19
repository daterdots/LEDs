
//Variables and such

#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

long current = 1;
long past = 1;
long temp = 1;
long count = 0;
int fibonacciMax = 100;

void setup() 
{
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
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
    while(!Serial.available()) 
    { 
      fibonacci();
    }
    current = 1;
    past = 1;
    temp = 1;
    count = 0;
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
/******************************************************************/
/******************************************************************/


















