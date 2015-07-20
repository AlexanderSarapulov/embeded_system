#include "Decimal.h"
#include <digitalWriteFast.h>
#include <GraphicsLib.h>
#include <fonts.h>
#include <BMPheader.h>
#include <SPI.h>

#include <MI0283QT2.h>
MI0283QT2 Display;

#include <Arduino.h>
#include <PS2Mouse.h>


#include <inttypes.h>
#define MOUSE_DATA 5
#define MOUSE_CLOCK 6


#include "colors.h"
#include "GuiButton.h"
//------------------------------------------------------------------------icludes are finished here---------------------------------------------------------


// ----------------------------------------------------------------------------- VAR definishion----------------------------------------------------------------
uint8_t counter = 0;
uint8_t counter2 = 0;
uint8_t counter3 = 0;
uint8_t counter4 = 0;
PS2Mouse mouse(MOUSE_CLOCK, MOUSE_DATA, STREAM);

  int left_button_pressed = 0;
  int rigth_button_pressed = 0;
  int global_status = 1 ;
  int mstat_before = 0;
  int prev_global_status = 0;
  int printing_x = 0;
  int printing_y = 0;
  int data[2];
  long total_distance_X = 0;
  long total_distance_Y = 0;
  float total_distance = 0;
  float circle_sq = 0;
  float rectangle_sq = 0;


// to use an anonymous function, you have to enable C++11 in arduino:
// http://stackoverflow.com/questions/16224746/how-to-use-c11-to-program-the-arduino

GuiButton foo1(10,10,50,80, "Distance",
              [](void) -> void{counter++;});
GuiButton foo2(80,10,50,80, "Circle sq", 
              [](void) -> void{counter2++;});   
GuiButton foo3(150,10,50,80, "Rect sq", 
              [](void) -> void{counter3++;});   
// ----------------------------------------------------------------------------- subprograms----------------------------------------------------------------


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

void print_header(char* header)
          {
          Display.setCursor(100, 30);
          Display.setTextSize(1);
          Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
          Display.print("                            ");
          Display.setCursor(100, 30);
          Display.println(header);
          Display.setCursor(100, 40);
          Display.setTextSize(2);
          Display.setTextColor(RGB(200,0,0), RGB(255,255,255));
          Display.print("                     ");
          Display.print("              ");
}

void print_data(float data, char* measure_units){
          Display.setCursor(100, 40);
          Display.setTextSize(2);
          Display.setTextColor(RGB(200,0,0), RGB(255,255,255));
          Display.print(data);
          Display.println(measure_units);
}

void print_data_2(float data, char* measure_units){
          Display.setCursor(100, 70);
          Display.setTextSize(1);
          Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
          Display.print("                            ");
          Display.setCursor(100, 70);
          Display.print(data);
          Display.println(measure_units);
}

void clear_print_data_2(){
          Display.setCursor(100, 70);
          Display.setTextSize(1);
          Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
          Display.print("                            ");
          Display.setCursor(100, 70);
          Display.print("                           ");
          Display.println("              ");
}

void print_message(char* message){
          Display.setCursor(100, 60);
          Display.setTextSize(1);
          Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
          Display.print("                            ");
          Display.setCursor(100, 60);
          Display.println(message);
}

void print_instruction(){
          Display.setCursor(100, 160);
          Display.setTextSize(1);
          Display.setTextColor(RGB(0,0,0), RGB(255,255,255));
          Display.println("Press left button to pause");
          Display.println("Press right button to reset");
}

void check_mouse_button(int mstat){

        if ((mstat == 9) and (mstat_before!=9))
        {
            if (left_button_pressed ==1) 
            {
                left_button_pressed = 0;
                clear_print_data_2();
            }
            else 
            {
                left_button_pressed = 1; 
            }
            mstat_before=9;
        } 
        else 
        {
            mstat_before = mstat;
        }
        if ((mstat == 10)) //and (mstat_before!=10))
        {
          rigth_button_pressed = 1;
        }
 }

//-----------------------------------------------------------------------------------SETUP starts here ------------------------------------------------------
void setup()
{
      Display.begin();
      Display.touchStartCal(); // without this command the touchscreen driver won't work properly.
      foo1.refresh();
      foo2.refresh();
      foo3.refresh();
   //   foo4.refresh();
      Serial.begin(9600);
      mouse.initialize();
      mouse_init();  
      print_instruction();
}


//-----------------------------------------------------------------------------------LOOP starts here ------------------------------------------------------
void loop()
{    
    Display.touchRead();
    //delay(50);
    static touchpanelEvent_t e;
    e.x = Display.touchX();
    e.y = Display.touchY();
    e.pressure = Display.touchZ();
    //Display.drawInteger(5,230,e.pressure,DEC,0x0000,0xFFFF,1);
    
    foo1.onTouchpanelEvent(e); //counter will stepup if press
    foo2.onTouchpanelEvent(e); //counter2 will stepup if press
    foo3.onTouchpanelEvent(e); //counter3 will stepup if press
    //showCounter1(a);
// ------------------------------------------------------- read buttons if there were pressed and decide which one was    
    if (counter > 0) {
      if (global_status != 1) {
            //foo4.refresh();
            global_status = 1;
      }
      counter = 0;
    }
    if (counter2 > 0) {
      if (global_status != 2) {
            //foo4.hide();
            global_status = 2;
      }
      counter2 = 0;
    }
    if (counter3 > 0) {
      if (global_status != 3) {
            //foo4.hide();
            global_status = 3;

      }
      counter3 = 0;
    }

//------------------------------------------------------------------------------IF some of buttons were pressed show for it's function    
    if (global_status == 1) 
    {
          if ((prev_global_status !=1 ) or (rigth_button_pressed > 0)) { //put initial setting for "Distance measurment mode"
                total_distance = 0;
                print_header ("Distance measurment mode");
                print_data(total_distance,"mm");
                counter4 = 0;
                prev_global_status = 1;
                total_distance_X = 0;
                total_distance_Y = 0;
                left_button_pressed = 0; 
                rigth_button_pressed = 0 ;
          }
          //foo4.onTouchpanelEvent(e);
          char mstat;
          char mx;
          char my;
          mouse.write(0xeb);  // give me data!
          mouse.read();      // ignore ack
          mstat = mouse.read();
          mx = mouse.read();
          my = mouse.read();
          if ((mstat == 9) and (mstat_before!=9))
          {
              if (left_button_pressed ==1) 
              {
                  left_button_pressed = 0;
                  clear_print_data_2();
              }
              else 
              {
                  left_button_pressed = 1; 
              }
              mstat_before=9;
          } 
          else 
          {
              mstat_before = mstat;
          }
          if ((mstat == 10)) //and (mstat_before!=10))
          {
            rigth_button_pressed = 1;
          } 
        
          /* send the data back up */
          //Serial.print(mstat, BIN);
//          Serial.print("\tX=");
//          Serial.print(total_distance_X);
//          Serial.print("\tY=");
//          Serial.print(total_distance_Y);
//          Serial.print("\totl =");
//          Serial.print(total_distance);
//          Serial.println();
//          delay(20);  /* twiddle */
               
               
          if(mx+my != 0) 
          {
 //          trigger=mx;
             printing_x = (mx);
             printing_y = (my);
             total_distance_X = total_distance_X + abs(mx);
             total_distance_Y = total_distance_Y + abs(my);
             total_distance = total_distance + sqrt(((mx*mx) + (my*my)))/147*21.8;
          }
          //print_data (total_distance,"mm");
          if (left_button_pressed !=1) 
          {
             print_data (total_distance," mm");
             print_message("       ");
          }
          else 
          {
             print_message("paused");
             print_data_2(total_distance," mm");
          }  

    }
//--------------------------------------------------------------------- button 2 was pressed----------------------------------------------------------        
    if (global_status == 2) 
    {
         if ((prev_global_status !=2 ) or (rigth_button_pressed > 0))  //put initial setting for "Circle sq mode" or reset theem if reset was pressed
         {
              total_distance = 0;
              //mstat_before = 0;
              circle_sq = 0;
              print_header ("Circle sq mode");
              print_data (circle_sq," cm^2");
              counter4 = 0;
              prev_global_status = 2;  
              total_distance_X = 0;
              total_distance_Y = 0;  
              left_button_pressed = 0; 
              rigth_button_pressed = 0 ;
        }
        //foo4.onTouchpanelEvent(e);
        char mstat;
        char mx;
        char my;
        mouse.write(0xeb);  // give me data!
        mouse.read();      // ignore ack
        mstat = mouse.read();
        mx = mouse.read();
        my = mouse.read();
        check_mouse_button(mstat);

        if( mx+my != 0) 
        {
           total_distance = total_distance + sqrt(((mx*mx) + (my*my)))/147*21.8;
           circle_sq = (total_distance*total_distance)/100/4*3.141592;
        }
        if (left_button_pressed !=1) 
        {
           //Serial.println(data[1],DEC); // X Movement Data
           print_data (circle_sq," cm^2");
           print_message("       ");
        }
        else 
        {
           print_message("paused");
           print_data_2(circle_sq," cm^2");
        }       
    }
//--------------------------------------------------------------------- button 3 was pressed----------------------------------------------------------    
     if (global_status == 3) 
     {
        if ((prev_global_status !=3 ) or (rigth_button_pressed > 0)) 
        {
             total_distance = 0;
             rectangle_sq = 0;
             print_header ("Rectangle sq mode");
             print_data (total_distance," cm^2");
             total_distance_X = 0;
             total_distance_Y = 0;
             prev_global_status = 3;
             left_button_pressed = 0; 
             rigth_button_pressed = 0 ;        
        }
        char mstat;
        char mx;
        char my;
        mouse.write(0xeb);  // give me data!
        mouse.read();      // ignore ack
        mstat = mouse.read();
        mx = mouse.read();
        my = mouse.read();
        check_mouse_button(mstat);
        if( (mx + my) != 0) 
        {
           total_distance_X = total_distance_X + abs(mx);
           total_distance_Y = total_distance_Y + abs(my);
           rectangle_sq = (total_distance_X*total_distance_Y)/147*21.8/10/147*21.8/10; //rectangle square formuls in centimeters
        }
       if (left_button_pressed != 1) 
       {
             //Serial.println(data[1],DEC); // X Movement Data
             print_data(rectangle_sq," cm^2");
             print_message("       ");
       }
       else 
       {
             print_message("paused");
             print_data_2(rectangle_sq," cm^2");
       }
    }    
    Display.touchRead();
}
