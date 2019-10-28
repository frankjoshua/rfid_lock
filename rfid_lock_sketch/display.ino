/**
 * 
 * oled 128x64 libs
 * https://github.com/adafruit/Adafruit_SSD1306
 * https://github.com/adafruit/Adafruit-GFX-Library
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icons.h"

#define OLED_RESET LED_BUILTIN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
}

void displayStatus(Status status){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  
  display.println(status.message);
  display.println(status.cardId);
  if(isLockEngaged()){
    if(isUnlocked()){
      display.println("Unlocked");
      display.drawBitmap(
      (display.width()  - UNLOCK_BMP_W),
      (display.height() - UNLOCK_BMP_H),
      UNLOCK_BMP, UNLOCK_BMP_W, UNLOCK_BMP_H, 1);    
    } else {
      display.println("Key Unlock");
       display.drawBitmap(
        (display.width()  - KEY_BMP_W),
        (display.height() - KEY_BMP_H),
        KEY_BMP, KEY_BMP_W, KEY_BMP_H, 1);  
    }     
  } else {
    display.println("Locked");
    display.drawBitmap(
      (display.width()  - LOCK_BMP_W),
      (display.height() - LOCK_BMP_H),
      LOCK_BMP, LOCK_BMP_W, LOCK_BMP_H, 1); 
  }
  if(status.timeTillLock > 0){
    display.println(status.timeTillLock / 1000);
  }
  display.display();

  Serial.println(status.state);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}
