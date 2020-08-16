#include "display.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Display::Display()
{
}

void Display::initDisplay()
{
  if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(2);
  oledDisplay.setTextColor(SSD1306_WHITE);
  oledDisplay.setCursor(0, 0);
  oledDisplay.cp437(true); // Use full 256 char 'Code Page 437' font

  oledDisplay.display();
}

void Display::println(String msg)
{
  oledDisplay.clearDisplay();
  oledDisplay.setCursor(0, 0);
  oledDisplay.println(msg);
  oledDisplay.display();
}

void Display::print(Status *status)
{
  String topLine;
  switch (status->mode)
  {
  case MODE_READ:
    topLine = "Tap Card";
    break;
  case MODE_UNLOCKED:
    topLine = "Be Safe";
    break;
  case MODE_CALL_WEBHOOK:
    topLine = "Card Lookup";
    break;
  case MODE_UPDATE:
    topLine = "Updating..";
    break;
  default:
    topLine = "Error";
    break;
  }
  IPAddress ip = WiFi.localIP();
  String lastLine = status->assetTag + "\n" + status->cardId + "." + String(ip[3]);
  oledDisplay.clearDisplay();
  oledDisplay.setCursor(0, 0);
  oledDisplay.setTextSize(2);
  oledDisplay.println(topLine);
  oledDisplay.println(status->msg);
  oledDisplay.println(lastLine);
  oledDisplay.display();
}