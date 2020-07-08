#include <Arduino.h>
#include "rfid_reader.h"
#include "backoff.h"
#include "display.h"
#include "current.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>

WiFiManager wifiManager;
RfidReader rfidReader;
BackoffTimer backoff;
Display display;
CurrentSensor currentSensor;

#define RELAY_PIN D0

#define TIME_TO_LOCK 6000
#define MODE_READ 0
#define MODE_CALL_WEBHOOK 1
#define MODE_UNLOCKED 2

unsigned long lockAtTime = 0;
int mode = MODE_READ;

String cardId = "";
String assetTag = "01001";

void relayOn(){
  digitalWrite(RELAY_PIN, LOW);
}

void relayOff(){
  digitalWrite(RELAY_PIN, HIGH);
}

void displayMessage(String msg){
  display.println(msg);
  Serial.println(msg);
}

void readCard(){
  if(rfidReader.isCardAvailable()) {
    cardId = rfidReader.getCardId();
    mode = MODE_CALL_WEBHOOK;
    backoff.reset();
    backoff.setDelay();
	}
}

void webhook(){
  if(WiFi.status() != WL_CONNECTED){
    displayMessage("No WIFI!");
    backoff.setDelay();
    return;
  }

  HTTPClient http;
  bool httpInitResult = http.begin("http://nodered.archreactor.net/webhook?card_id=" + cardId + "&asset_tag=" + assetTag);
  if(!httpInitResult){
    displayMessage("Could not init http!");
    backoff.setDelay();
    return;
  }

  int httpCode = http.GET();
  switch (httpCode)
  {
  case 404: // User not found
  case 401: // User not authorized
  case 500: // Server Error Unknown
    displayMessage("Access Denied: " + String(httpCode));
    mode = MODE_READ;
    backoff.reset();
    break;
  case 200:
    displayMessage("Unlocked");
    mode = MODE_UNLOCKED;
    lockAtTime = millis() + TIME_TO_LOCK;
    backoff.reset();
    break;
  default:
    char error[256];
    sprintf(error, "HTTP GET failed, error: %s\n%d", http.errorToString(httpCode).c_str(), httpCode);
    displayMessage(error);
    break;
  }

  http.end();
  backoff.setDelay();
}

void unlockLoop(){
  backoff.reset();
  backoff.setDelay();
  if(currentSensor.getCurrentInAmps() > 1.0){
    lockAtTime = millis() + TIME_TO_LOCK;
  }
  int timeLeft = (lockAtTime - millis()) / 1000;
  if(timeLeft > 0){
    relayOn();
    displayMessage("Locking in " + String(timeLeft) + " " + String(currentSensor.getCurrentInAmps()));
  }
  else {
    relayOff();
    displayMessage("Tap Card");
    mode = MODE_READ;
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  display.initDisplay();
  displayMessage("Wifi <<*>>");
  wifiManager.autoConnect("ArchReactorLockout");
  displayMessage("Tap Card");
  pinMode(RELAY_PIN, OUTPUT);
  relayOff();
}

void loop() {
  if(backoff.isReady()){
    switch(mode){
      case MODE_READ:
        readCard();
        break;
      case MODE_CALL_WEBHOOK:
        webhook();
        break;
      case MODE_UNLOCKED:
        unlockLoop();
        break;
    }
  }
}