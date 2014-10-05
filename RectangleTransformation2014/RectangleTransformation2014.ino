//  RectangleTransformation2014 is a program that lets you determine the borders of a rectangle
//  transforming into a triangle via an intermediate trapezoid in a 2D matrix
//
//  Daniel Wilson, 2014

#include <math.h>

#define h_0   60    // rectange starting height
#define w_0   60    // rectange starting width
#define h_v   35    // vanishing point height

const double theta = atan(w_0 / 2.0 / (h_0 - h_v));
int h_i = h_0;      // h_i is 
int w_i = w_0;
int h = h_i;
int g_ih = 0;
long t1 = 0;
long t2 = 0;

void setup(){
  Serial.begin(115200); 
}

void loop(){
  t1 = millis();
  for (h_i = h_0; h_i >= h_v; h_i--)    // start the height of the trapezoid at rectange height
  { 
    w_i = 2 * tan(theta) * (h_i - h_v);    // the width of the top of the trapezoid
    Serial.println();
    Serial.print(" the height of the trapezoid is: ");
    Serial.println(h_i);
    Serial.print(" the width of the top of the trapezoid is: ");
    Serial.println(w_i);
    Serial.println();

    for(int h = h_i; h >= 0; h--){
      g_ih = (w_0 - w_i) / 2 * h / h_0;    // the width of the gap at heigh h

      // print the the format row ; starting light ; ending light  
      Serial.print(h);                // this is the row of LEDs (counting from the bottom)
      Serial.print(" ; ");
      Serial.print(g_ih);             // this is the index of the first LED in the row that should be lit up
      Serial.print(" ; ");
      Serial.println(w_0 - g_ih);     // this is the index of the last LED in the row that should be lit up
    }
  }
  t2 = millis();
  Serial.print("it took this long to calculate bounds of one complete transformation (in ms): ");
  Serial.println(t2 - t1);
  delay(1000);
}










