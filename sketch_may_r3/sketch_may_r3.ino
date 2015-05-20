#include <Arduino.h>
#include <PS2Mouse.h>
#include <ps2.h>

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
int data[2];
int16_t trigger = 0;
  int total_distance_X = 0;
  int total_distance_Y = 0;
  float total_distance = 0;


void mouse_init()
{
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}

int binary2decimal(byte b)
{

 int dec = 0;
 int power = 1;
 byte mask; 
 int weight;
 
 for (mask = 0x01; mask; mask <<= 1)
 {
   if (b & mask)
   {
     weight = 1;     
   }
   else
   {
     weight = 0;     
   }
   
   dec = dec + (power * weight);   
   power = power * 2; 
   
}
 
 return dec;
}


void setup() {
  Display.begin();
  //Display.write("Knock, knock, Neo");
  Serial.begin(9600);
  mouse.initialize();
  mouse_init();
  PS2 mouse(6, 5);


}




void loop() {
  
  
  char mstat;
  char mx;
  char my;
  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();
  mx = mouse.read();
  my = mouse.read();

  /* send the data back up */
  Serial.print(mstat, BIN);
  Serial.print("\tX=");
  Serial.print(total_distance_X);
  Serial.print("\tY=");
  Serial.print(total_distance_Y);
  Serial.print("\totl =");
  Serial.print(total_distance);
  Serial.println();
//  delay(20);  /* twiddle */
 
 if(mx+my != 0) 
 {
   trigger=mx;
   printing_x = (mx);
   printing_y = (my);
   total_distance_X = total_distance_X + abs(mx);
   total_distance_Y = total_distance_Y + abs(my);
   total_distance = total_distance + sqrt(((mx*mx) + (my*my)))/147*21.8;
 }
 
// Serial.println(data[1],DEC); // X Movement Data
  Display.setCursor(8, 30);
  Display.setTextSize(1);
  Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
  Display.println("Total distance run");
  Display.setCursor(8, 40);
  Display.setTextSize(2);
  Display.setTextColor(RGB(200,0,0), RGB(255,255,255));
  Display.print(total_distance);
  Display.println(" mm");
  Display.setCursor(8, 60);
  Display.setTextColor(RGB(0,200,0), RGB(255,255,255));
  Display.print("test data");
  Display.setCursor(8, 80);
  Display.setTextColor(RGB(0,200,0), RGB(255,255,255));
  Display.print("Y coordinate  ");
  Display.print(printing_y);
  Display.setCursor(8, 100);
  Display.setTextColor(RGB(0,200,0), RGB(255,255,255));
  Display.print("X coordinate  ");
  Display.print(printing_x);
}

