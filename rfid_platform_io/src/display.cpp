#include "display.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Display::Display(){
  // // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // if(!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
  //   // Serial.println(F("SSD1306 allocation failed"));
  //   for(;;); // Don't proceed, loop forever
  // }

}

void Display::initDisplay(){
  if(!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(2);
  oledDisplay.setTextColor(SSD1306_WHITE);
  oledDisplay.setCursor(0, 0);
  oledDisplay.cp437(true); // Use full 256 char 'Code Page 437' font
  
  oledDisplay.println("01234\n567890123456789");
  oledDisplay.display();
}

void Display::println(String msg){
  oledDisplay.clearDisplay();
  oledDisplay.setCursor(0, 0);
  oledDisplay.println(msg);
  oledDisplay.display();
}