/**
 * Requirements:
 * https://github.com/tzapu/WiFiManager
 * https://github.com/miguelbalboa/rfid
 * https://github.com/adafruit/Adafruit_SSD1306
 * https://github.com/adafruit/Adafruit-GFX-Library
 */

#include "status.h"

const String AP_NAME = "RFIDLOCK_1";

void setup() {
  Serial.begin(9600);

  delay(100);
  initDisplay();
  setMessage("Booting...");
  delay(1000);
  initRFIDReader();
  initLock();
  String msg = "No Wifi!\nAP SSID:\n" + AP_NAME;
  setMessage(msg);
  initWifi(AP_NAME);
  setMessage("Updating Card Database");
  updateRepository();
  setMessage("RFID Lock");
}

void loop() {
  long timeElapsed = getTimeElapsed();

  if(isUnlocked()){
    updateTimeTillLock(timeElapsed);
    if(isTimeToLock()){
      setLocked();
    }
  }
  
  if(isCardPresent()){
    updateCardId();
    if(isCardAuthorized()){
      setUnlocked();
    } else {
      setLocked();
      updateRepository();
    }
  }

  if(shouldUpdateState()){
    updateLockAndDisplay();
  }

}
