#include <Arduino.h>
#include <PS2Mouse.h>

#include <inttypes.h>
#include <SPI.h>
#include <digitalWriteFast.h>
#include <BMPheader.h>
#include <fonts.h>
#include <GraphicsLib.h>
#include <MI0283QT2.h>
#define MOUSE_DATA 5
#define MOUSE_CLOCK 6

PS2Mouse mouse(MOUSE_CLOCK, MOUSE_DATA, STREAM);


MI0283QT2 Display;
int printing_x = 0;
int printing_y = 0;
int16_t trigger = 0;

void setup() {
  Display.begin();
  Display.write("Knock, knock, Neo");
//Serial.begin(38400);
 Serial.begin(9600);
  mouse.initialize();
  

}

void loop() {
  int data[2];
  mouse.report(data);
  //Serial.print(data[0]); // Status Byte
  //Serial.print(":");
  //Serial.println(data[1],DEC); // X Movement Data
  //Serial.print("\n");
  // Serial.print(",");
 //Serial.print(data[2]); // Y Movement Data
 
 // Display.write(data[1]+" "+data[2]);
 // Serial.println();
 if(trigger != data[1]) 
 {
   printing_x++;
   printing_y++;
   trigger=data[1];
   Serial.println(printing_x); // X Movement Data
   
 }
 
 Serial.println(data[1],DEC); // X Movement Data
  Display.setCursor(15, 30);
  Display.setTextSize(1);
  Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
  Display.println("Temperature");
  Display.setCursor(8, 40);
  Display.setTextSize(2);
  Display.setTextColor(RGB(200,0,0), RGB(255,255,255));
  Display.print(printing_x);
  Display.println(" C");
  Display.setCursor(8, 60);
  Display.setTextColor(RGB(0,200,0), RGB(255,255,255));
  Display.print("fahrenheit");
  Display.print(" F");
  Display.setCursor(8, 80);
  Display.setTextColor(RGB(0,200,0), RGB(255,255,255));
  Display.print("Y coordinates  ");
  Display.print(trigger);
}

