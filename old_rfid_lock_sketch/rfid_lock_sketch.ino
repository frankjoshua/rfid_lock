/**
 * Requirements:
 * https://github.com/tzapu/WiFiManager
 * https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino
 * https://github.com/adafruit/Adafruit_SSD1306
 * https://github.com/adafruit/Adafruit-GFX-Library
 * https://github.com/bblanchon/ArduinoJson
 */
#include "status.h"
#include <ESP8266WiFi.h>

//const char* AP_NAME = "RFIDLOCK_1";

void setup() {
  Serial.begin(9600);

  delay(100);
  initDisplay();
  setMessage("Booting...");
  delay(1000);
  initRFIDReader();
  initLock();
  String msg = String("No Wifi!\nAP SSID:\n");
  setMessage(msg);
  initWifi();
//  setMessage("Updating Card Database");
//  updateRepository();
  initFiles();
  setMessage(WiFi.localIP().toString());
}

void loop() {
  serverLoop();
  
  long timeElapsed = getTimeElapsed();

  if(isUnlocked()){
    updateTimeTillLock(timeElapsed);
    if(isTimeToLock()){
      setLocked();
    }
  }
  
  if(isCardPresent()){
//    updateCardId();
//    if(isCardAuthorized()){
//      setMessage("Authorized");
//      setUnlocked();
//    } else {
//      setLocked();
//      updateRepository(getApiKey(), getUserKey(), currentCardId());
//      if(isCardAuthorized()){
//        setMessage("Authorized");
//        setUnlocked();
//      }
//    }
  }

  if(shouldUpdateState()){
    updateLockAndDisplay();
  }

}
