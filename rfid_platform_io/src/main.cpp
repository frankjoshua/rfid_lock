#include <Arduino.h>
#include "rfid_reader.h"
#include "backoff.h"
#include "display.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>

WiFiManager wifiManager;
RfidReader rfidReader;
BackoffTimer backoff;
Display display;

#define MODE_READ 0
#define MODE_CALL_WEBHOOK 1
int mode = MODE_READ;

String cardId = "";

void displayMessage(String msg){
  display.println(msg);
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
    backoff.setDelay();
    return;
  }
  HTTPClient http;
  bool httpInitResult = http.begin("http://nodered.archreactor.net/webhook?card_id=" + cardId);
  if(!httpInitResult){
    displayMessage("Could not init http!");
    backoff.setDelay();
    return;
  }
  int httpCode = http.GET();
  if(httpCode == 200){
    mode = MODE_READ;
    backoff.reset();
  } else {
    char error[256];
    sprintf(error, "HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    displayMessage(error);
  }
  http.end();
  backoff.setDelay();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  display.initDisplay();
  displayMessage("Ready...");
  wifiManager.autoConnect("ArchReactorLockout");
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
    }  
  }
}