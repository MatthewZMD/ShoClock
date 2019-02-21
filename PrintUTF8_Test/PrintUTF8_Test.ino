#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*
  U8glib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.   
*/

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

void setup(void) {
  u8g2.begin();
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
}

void loop(void) {
  u8g2.setFont(u8g2_font_6x10_tf);  // use font
  u8g2.setFontDirection(0); // set font print direction
  u8g2.clearBuffer(); // clear screen
  u8g2.setCursor(0, 15); // set printing position
  u8g2.print("anal"); // set print content
  u8g2.drawBox(5,10,20,10); // duh
  u8g2.drawLine(7, 10, 40, 55); // draw line segment with coordinates
  u8g2.sendBuffer(); // display screen
  
  delay(1000);
}
