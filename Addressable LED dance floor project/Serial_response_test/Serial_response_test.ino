
//Variables and such

int current = 1;
int past = 1;
int temp = 1;
int count = 0;

void setup() 
{
  Serial.begin(115200);
}

void loop() 
{
  checkSerial();
}

/*************************
 * THESE ARE THE SUBROUTINES
 *************************/

/******************************************************************/
void checkSerial()
{
  if(!Serial.available()) //if there is nothing in the serial port, go back to void loop
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
  Serial.println(" j - ");
  Serial.print('\n'); 
}

/******************************************************************/
void fibonacci() 
{
  //Serial.print("The value of the sequence is now ");
  //Serial.println(current);
  if(current<300)
  {
    while (count<current)
    {
      Serial.print("|"); 
      delay (20);
      count++;
    }
    Serial.print("\n"); 
    temp = current;
    current += past;
    past = temp;
    count = 0;
    delay (500);
  }
}

