/*
  SSD1331 OLED-Display Lib for Arduino
  by Watterott electronic (www.watterott.com)
 */

#include <inttypes.h>
#if defined(__AVR__)
# include <avr/io.h>
# include <avr/pgmspace.h>
#else
# define pgm_read_byte(addr)  (*(const uint8_t *)(addr))
# define pgm_read_word(addr)  (*(const uint16_t *)(addr))
# define pgm_read_dword(addr) (*(const uint32_t *)(addr))
#endif
#if ARDUINO >= 100
# include "Arduino.h"
#else
# include "WProgram.h"
#endif
#include "SPI.h"
#include "digitalWriteFast.h"
#include "GraphicsLib.h"
#include "SSD1331.h"


//#define SOFTWARE_SPI //use software SPI on pins 11,12,13

#ifndef LCD_WIDTH
# define LCD_WIDTH      (96)
# define LCD_HEIGHT     (64)
#endif

#if (defined(__AVR_ATmega1280__) || \
     defined(__AVR_ATmega1281__) || \
     defined(__AVR_ATmega2560__) || \
     defined(__AVR_ATmega2561__))      //--- Arduino Mega ---

# define RST_PIN        (8)
# define CS_PIN         (7)
# define RS_PIN         (5)
# if defined(SOFTWARE_SPI)
#  define MOSI_PIN      (11)
#  define CLK_PIN       (13)
# else
#  define MOSI_PIN      (51)
#  define CLK_PIN       (52)
# endif

#elif (defined(__AVR_ATmega644__) || \
       defined(__AVR_ATmega644P__))    //--- Arduino 644 (www.mafu-foto.de) ---

# define RST_PIN        (12)
# define CS_PIN         (13)
# define RS_PIN         (15)
# define MOSI_PIN       (5)
# define CLK_PIN        (7)

#elif defined(__AVR_ATmega32U4__)      //--- Arduino Leonardo ---

# define RST_PIN        (8)
# define CS_PIN         (7)
# define RS_PIN         (5)
# if defined(SOFTWARE_SPI)
#  define MOSI_PIN      (11)
#  define CLK_PIN       (13)
# else
#  define MOSI_PIN      (16) //PB2
#  define CLK_PIN       (15) //PB1
# endif

#else                                  //--- Arduino Uno ---

# define RST_PIN        (8) //8, Adafruit: 9
# define CS_PIN         (7) //7, Adafruit: 10
# define RS_PIN         (5) //5, Adafruit: 8
# define MOSI_PIN       (11)
# define CLK_PIN        (13)

#endif


#define RST_DISABLE()   digitalWriteFast(RST_PIN, HIGH)
#define RST_ENABLE()    digitalWriteFast(RST_PIN, LOW)

#define CS_DISABLE()    digitalWriteFast(CS_PIN, HIGH)
#define CS_ENABLE()     digitalWriteFast(CS_PIN, LOW)

#define RS_HIGH()       digitalWriteFast(RS_PIN, HIGH)
#define RS_LOW()        digitalWriteFast(RS_PIN, LOW)

#define MOSI_HIGH()     digitalWriteFast(MOSI_PIN, HIGH)
#define MOSI_LOW()      digitalWriteFast(MOSI_PIN, LOW)

#define CLK_HIGH()      digitalWriteFast(CLK_PIN, HIGH)
#define CLK_LOW()       digitalWriteFast(CLK_PIN, LOW)


//LCD commands
#define LCD_CMD_DRAWLINE               0x21
#define LCD_CMD_DRAWRECT               0x22
#define LCD_CMD_COPY                   0x23
#define LCD_CMD_DIMWINDOW              0x24
#define LCD_CMD_CLEARWINDOW            0x25
#define LCD_CMD_FILL                   0x26
#define LCD_CMD_SCROLL                 0x27
#define LCD_CMD_SCROLL_OFF             0x2E
#define LCD_CMD_SCROLL_ON              0x2F
#define LCD_CMD_COLUMN                 0x15
#define LCD_CMD_ROW                    0x75
#define LCD_CMD_CONTRASTA              0x81
#define LCD_CMD_CONTRASTB              0x82
#define LCD_CMD_CONTRASTC              0x83
#define LCD_CMD_MASTERCURRENT          0x87
#define LCD_CMD_PRECHARGEA             0x8A
#define LCD_CMD_PRECHARGEB             0x8B
#define LCD_CMD_PRECHARGEC             0x8C
#define LCD_CMD_REMAP                  0xA0
#define LCD_CMD_STARTLINE              0xA1
#define LCD_CMD_DISPLAYOFFSET          0xA2
#define LCD_CMD_NORMALDISPLAY          0xA4
#define LCD_CMD_DISPLAYALL_ON          0xA5
#define LCD_CMD_DISPLAYALL_OFF         0xA6
#define LCD_CMD_INVERTDISPLAY          0xA7
#define LCD_CMD_SETMULTIPLEX           0xA8
#define LCD_CMD_SETMASTER              0xAD
#define LCD_CMD_DISPLAY_OFF            0xAE
#define LCD_CMD_DISPLAY_ON             0xAF
#define LCD_CMD_POWERMODE              0xB0
#define LCD_CMD_PRECHARGE              0xB1
#define LCD_CMD_CLOCKDIV               0xB3
#define LCD_CMD_PRECHARGELEVEL         0xBB
#define LCD_CMD_NOP                    0xBC //0xBC, 0xBD, 0xE3
#define LCD_CMD_VCOMH                  0xBE


//-------------------- Constructor --------------------


SSD1331::SSD1331(void) : GraphicsLib(LCD_WIDTH, LCD_HEIGHT)
{
  return;
}


//-------------------- Public --------------------


void SSD1331::begin(uint_least8_t clock_div)
{
  //init pins
#if defined(RST_PIN)
  pinMode(RST_PIN, OUTPUT);
  RST_ENABLE();
#endif
  pinMode(CS_PIN, OUTPUT);
  CS_DISABLE();
  pinMode(RS_PIN, OUTPUT);
  RS_HIGH();
  pinMode(CLK_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);

  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(clock_div);
  SPI.begin();

  //reset display
  reset(clock_div);

  return;
}


void SSD1331::begin(void)
{
  return begin(SPI_CLOCK_DIV4);
}


void SSD1331::invertDisplay(uint_least8_t invert)
{
  if(invert == 0)
  {
     wr_cmd(LCD_CMD_NORMALDISPLAY);
  }
  else
  {
     wr_cmd(LCD_CMD_INVERTDISPLAY);
  }

  return;
}


void SSD1331::setOrientation(uint_least16_t o)
{
  uint_least8_t p;

       if((o ==   9) || 
          (o ==  90))
  {
    lcd_orientation = 90;
    lcd_width  = LCD_HEIGHT;
    lcd_height = LCD_WIDTH;
    p = 0x71; //0x71=RGB, 0x75=BGR
  }

  else if((o ==  18) || 
          (o == 180))
  {
    lcd_orientation = 180;
    lcd_width  = LCD_WIDTH;
    lcd_height = LCD_HEIGHT;
    p = 0x60; //0x60=RGB, 0x64=BGR
  }

  else if((o ==  27) || 
          (o ==  14) || 
          (o == 270))
  {
    lcd_orientation = 270;
    lcd_width  = LCD_HEIGHT;
    lcd_height = LCD_WIDTH;
    p = 0x63; //0x63=RGB, 0x67=BGR
  }

  else //0
  {
    lcd_orientation = 0;
    lcd_width  = LCD_WIDTH;
    lcd_height = LCD_HEIGHT;
    p = 0x72; //0x72=RGB, 0x76=BGR
  }

  wr_cmd(LCD_CMD_REMAP);
  wr_cmd(p);

  setArea(0, 0, lcd_width-1, lcd_height-1);

  return;
}


void SSD1331::setArea(int_least16_t x0, int_least16_t y0, int_least16_t x1, int_least16_t y1)
{
  if((lcd_orientation ==  90) ||
     (lcd_orientation == 270))
  {
    wr_cmd(LCD_CMD_COLUMN);
    wr_cmd(y0);
    wr_cmd(y1);
    wr_cmd(LCD_CMD_ROW);
    wr_cmd(x0);
    wr_cmd(x1);
  }
  else  //0 or 180
  {
    wr_cmd(LCD_CMD_COLUMN);
    wr_cmd(x0);
    wr_cmd(x1);
    wr_cmd(LCD_CMD_ROW);
    wr_cmd(y0);
    wr_cmd(y1);
  }

  return;
}


void SSD1331::drawStart(void)
{
  //RS_HIGH(); //data
  CS_ENABLE();

  return;
}


void SSD1331::draw(uint_least16_t color)
{
  wr_spi(color>>8);
  wr_spi(color);

  return;
}


void SSD1331::drawStop(void)
{
  CS_DISABLE();

  return;
}


//-------------------- Private --------------------


#if defined(__AVR__)
//const PROGMEM uint8_t initdata1331[] = 
const uint8_t initdata1331[] PROGMEM = 
#else
const uint8_t initdata1331[] = 
#endif
{
  0x40| 1, LCD_CMD_DISPLAY_OFF,
  0x40| 2, LCD_CMD_REMAP, 0x72, //0x72=RGB, 0x76=BGR
  0x40| 3, LCD_CMD_COLUMN, 0, LCD_WIDTH-1,
  0x40| 3, LCD_CMD_ROW, 0, LCD_HEIGHT-1,
  0x40| 2, LCD_CMD_STARTLINE, 0x00,
  0x40| 2, LCD_CMD_DISPLAYOFFSET, 0x00,
  0x40| 1, LCD_CMD_NORMALDISPLAY,
  0x40| 2, LCD_CMD_SETMULTIPLEX, 0x3F,
  0x40| 2, LCD_CMD_SETMASTER, 0x8E,
  0x40| 2, LCD_CMD_POWERMODE, 0x0B,
  0x40| 2, LCD_CMD_PRECHARGE, 0x31,
  0x40| 2, LCD_CMD_CLOCKDIV, 0xF0,
  0x40| 2, LCD_CMD_PRECHARGEA, 0x64,
  0x40| 2, LCD_CMD_PRECHARGEB, 0x78,
  0x40| 2, LCD_CMD_PRECHARGEC, 0x64,
  0x40| 2, LCD_CMD_PRECHARGELEVEL, 0x3A,
  0x40| 2, LCD_CMD_VCOMH, 0x3E,
  0x40| 2, LCD_CMD_MASTERCURRENT, 0x06,
  0x40| 2, LCD_CMD_CONTRASTA, 0x91,
  0x40| 2, LCD_CMD_CONTRASTB, 0x50,
  0x40| 2, LCD_CMD_CONTRASTC, 0x7D,
  0x40| 1, LCD_CMD_DISPLAY_ON,
  0xC0|20, //20ms
  0xFF   , 0xFF
};


void SSD1331::reset(uint_least8_t clock_div)
{
  uint_least8_t c, i;
#if defined(__AVR__)
  const PROGMEM uint8_t *ptr;
#else
  const uint8_t *ptr;
#endif

  //SPI speed-down
#if !defined(SOFTWARE_SPI)
  SPI.setClockDivider(SPI_CLOCK_DIV8);
#endif

  //reset
  CS_DISABLE();
#if defined(RST_PIN)
  RST_ENABLE();
  delay(50);
  RST_DISABLE();
  delay(120);
#endif

  //send init commands and data
  ptr = &initdata1331[0];
  while(1)
  {
    c = pgm_read_byte(ptr++);
    if(c == 0xFF) //end of data
    {
      break;
    }
    switch(c&0xC0)
    {
      case 0x40: //command
        for(i=c&0x3F; i!=0; i--)
        {
          c = pgm_read_byte(ptr++);
          wr_cmd(c);
        }
        break;
      case 0xC0: //delay
        c = c&0x3F;
        delay(c);
        break;
    }
  }

  //clear display buffer
  fillScreen(0);

  //restore SPI settings
#if !defined(SOFTWARE_SPI)
  SPI.setClockDivider(clock_div);
#endif

  return;
}


void SSD1331::wr_cmd(uint_least8_t cmd)
{
  RS_LOW(); //cmd
  CS_ENABLE();

  wr_spi(cmd);

  CS_DISABLE();
  RS_HIGH(); //data

  return;
}


void SSD1331::wr_data(uint_least8_t data)
{
  //RS_HIGH(); //data
  CS_ENABLE();

  wr_spi(data);

  CS_DISABLE();

  return;
}


void SSD1331::wr_spi(uint_least8_t data)
{
#if defined(SOFTWARE_SPI)
  for(uint_least8_t mask=0x80; mask!=0; mask>>=1)
  {
    CLK_LOW();
    if(mask & data)
    {
      MOSI_HIGH();
    }
    else
    {
      MOSI_LOW();
    }
    CLK_HIGH();
  }
  CLK_LOW();
#else
  SPI.transfer(data);
#endif
  return;
}
