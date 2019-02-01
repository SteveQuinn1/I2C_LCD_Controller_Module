// Copyright 2018 Steve Quinn
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
// Requires LiquidCrystal_I2C_PCF8574 Library found here:         https://github.com/SteveQuinn1/LiquidCrystal_I2C_PCF8574
// Requires MCP4561 Library found here:                           https://github.com/SteveQuinn1/MCP4561_DIGI_POT


#include "MCP4561_DIGI_POT.h"
#include <Wire.h>

#define BACKLIGHT_I2C_ADDRESS                   0x08
#define DISPLAY_BACKLIGHT_UPPER_VALUE_DEFAULT   ((unsigned long) (255))
#define DISPLAY_BACKLIGHT_LOWER_VALUE_DEFAULT   ((unsigned long) (10))
#define DISPLAY_CONTRAST_VALUE_DEFAULT          ((uint8_t)       (40))



#include <LiquidCrystal_I2C_PCF8574.h>
//MCP4561 digitalPot(0x2E);                      // Instance of MCP4561 pot with address supplied. A0 = 0
MCP4561 digitalPot;                              // Instance of MCP4561 pot with default address. A0 = 0


#define MAX_MESSAGE_SIZE 150
char cMessage[MAX_MESSAGE_SIZE];
uint8_t u8LCDBrightnessVal;
uint8_t u8LCDContrastVal;

uint8_t u8LCDColumnVal;
uint8_t u8RowVal;
#define MAX_LCD_DISPLAY_LINE_SIZE 20+1
char cLCDStringVal[MAX_LCD_DISPLAY_LINE_SIZE];
uint8_t u8Index;
char ch;

// Instantiate system LCD display
#define MAX_SYSTEM_LCD_COLUMNS 20
#define MAX_SYSTEM_LCD_ROWS    4
LiquidCrystal_I2C_PCF8574 lcd(0x3F,MAX_SYSTEM_LCD_COLUMNS,MAX_SYSTEM_LCD_ROWS);  // set the LCD address to 0x3F for a 20 chars and 4 line display





void setBackLightLevel(uint8_t blVal)
{
    Wire.beginTransmission(BACKLIGHT_I2C_ADDRESS);
    Wire.write((byte)(blVal));
    Wire.endTransmission();
}





void setup(void)
{
  Wire.begin();                                   
  Serial.begin(115200);          
  // Initialise system LCD display
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  setBackLightLevel(DISPLAY_BACKLIGHT_UPPER_VALUE_DEFAULT);
  lcd.setCursor(0,0);
  lcd.writeStr("<== Hello World 0==>"); // print message
  lcd.setCursor(0,1);
  lcd.writeStr("<== Hello World 1==>"); // print message
  lcd.setCursor(0,2);
  lcd.writeStr("<== Hello World 2==>"); // print message
  lcd.setCursor(0,3);
  lcd.writeStr("<== Hello World 3==>"); // print message
  digitalPot.writeVal(MCP4561_VOL_WIPER_0,DISPLAY_CONTRAST_VALUE_DEFAULT);
  putMenu();                                  
} 


void loop(void)
{
  char c;

  if (Serial.available())
    c = Serial.read();

  //delay(5);
  switch(c) {
    case 'm':
        putMenu();
        Serial.flush();
      break;
    
    case 'c':
        uint16_t u16Result;
        u8LCDContrastVal = Serial.parseInt();
        u16Result = digitalPot.writeVal(MCP4561_VOL_WIPER_0,u8LCDContrastVal);
         if (u16Result == MCP4561_SUCCESS)             
         {
           sprintf(cMessage," c - LCD contrast set to Value : %d",u8LCDContrastVal);               
           Serial.println(cMessage);
         } 
         else
           Serial.println(F(" c - failed to set new LCD contrast value"));  
         Serial.flush();
      break;

    case 'b':
         u8LCDBrightnessVal = ((uint8_t) Serial.parseInt());
         setBackLightLevel(u8LCDBrightnessVal);
         sprintf(cMessage," b - Set backlight level value : %d",u8LCDBrightnessVal);               
         Serial.println(cMessage);
         Serial.flush();
      break;

    case 's':
         lcd.clear();
         sprintf(cMessage," s - Clear LCD display");               
         Serial.println(cMessage);
         Serial.flush();
      break;
     
    case 'p':
         u8LCDColumnVal = ((uint8_t) Serial.parseInt());
         u8RowVal       = ((uint8_t) Serial.parseInt());
         u8Index        = 0;
         cLCDStringVal[u8Index] = '\0';
         ch             = Serial.read(); // remove the last comma
         ch             = '\0';
         
         delay(3); // Slight delay to all more chars to arrive
         while (Serial.available() > 0) {
           delay(3); // Slight delay to all more chars to arrive

           // read each text char and store in buffer to print when CR/LF detected.
           ch = Serial.read();
           if ((ch != '\n') && (ch != '\r')) {
              cLCDStringVal[u8Index++] = ch;
              cLCDStringVal[u8Index] = '\0';
           }
         }
         // Send test to LCD
         lcd.setCursor(u8LCDColumnVal,u8RowVal);
         lcd.writeStr(cLCDStringVal); // print message

         // Send response to serial port
         sprintf(cMessage," pC,R,Text - Print Text at Column/Row : %d,%d,%s",u8LCDColumnVal,u8RowVal,cLCDStringVal);               
         Serial.println(cMessage);
         Serial.flush();
      break;

  
  }
}


void putMenu(void)
{
  Serial.println();
  Serial.println(F(" Multifunction LCD display controller software "));
  Serial.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
  Serial.println(F("m         : This Menu                          "));
  Serial.println(F("cXXX      : Set contrast level XXX (000 - 255) "));
  Serial.println(F("bXXX      : Set backlight level XXX (000 - 255)"));
  Serial.println(F("s         : Clear LCD display                  "));
  Serial.println(F("pC,R,Text : Print Text at Row/Column           "));
  Serial.println(F("-----------------------------------------------"));
}
