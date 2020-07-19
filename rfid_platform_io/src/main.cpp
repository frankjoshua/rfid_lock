#include <Arduino.h>
#include "rfid_reader.h"
#include "backoff.h"
#include "display.h"
#include "current.h"
#include "status.h"
#include "storage.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
RfidReader rfidReader;
BackoffTimer backoff;
BackoffTimer currentSensorBackoff;
Display display;
CurrentSensor currentSensor;
Status status;
Storage storage;

#define RELAY_PIN D0

#define TIME_TO_LOCK 5 * 60 * 1000

unsigned long lockAtTime = 0;

void relayOn(){
  digitalWrite(RELAY_PIN, HIGH);
}

void relayOff(){
  digitalWrite(RELAY_PIN, LOW);
}

void displayMessage(String msg){
  status.msg = msg;
  display.print(&status);
  Serial.println(msg);
}

void readCard(){
  if(millis() % 1000 == 0){
    displayMessage("");
  }
  if(rfidReader.isCardAvailable()) {
    status.cardId = rfidReader.getCardId();
    status.mode = MODE_CALL_WEBHOOK;
    backoff.reset();
    backoff.setDelay();
	}
}

void unlock() {
  status.mode = MODE_UNLOCKED;
  lockAtTime = millis() + TIME_TO_LOCK;
  backoff.reset();
}

void webhook(){
  if(WiFi.status() != WL_CONNECTED){
    displayMessage("No WIFI!");
    backoff.setDelay();
    return;
  }

  HTTPClient http;
  bool httpInitResult = http.begin("http://nodered.archreactor.net/webhook?card_id=" + status.cardId + "&asset_tag=" + status.assetTag);
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
    status.mode = MODE_READ;
    backoff.reset();
    break;
  case 200:
    unlock();
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

String timeToString(unsigned long t){
 static char str[5];
 t = t % 3600;
 int m = t / 60;
 int s = t % 60;
 sprintf(str, "%01d:%02d", m, s);
 return String(str);
}

void lock(){
  relayOff();
  status.mode = MODE_READ;
}

void unlockLoop(){
  backoff.reset();
  backoff.setDelay();
  if(status.current > 1.0){
    lockAtTime = millis() + TIME_TO_LOCK;
  }
  int timeLeft = (lockAtTime - millis()) / 1000;
  if(timeLeft > 0){
    relayOn();
    displayMessage(timeToString(timeLeft) + " " + String(status.current) + "A");
  } else {
    lock();
  }
}

void updateLoop() {
  unsigned long time = millis();
  if(time % 1500 == 0){
    displayMessage("......");
  } else if(time % 1000 == 0){
    displayMessage("....");
  } else if(time % 500 == 0){
    displayMessage(".");
  }  
  AsyncElegantOTA.loop();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  delay(2000);
  storage.restore(&status);
  Serial.println("Initalizing display.");
  display.initDisplay();
  displayMessage("Wifi <<*>>");
  wifiManager.autoConnect("ArchReactorLockout");
  displayMessage("");
  pinMode(RELAY_PIN, OUTPUT);
  relayOff();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    status.current = currentSensor.getCurrentInAmps();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(request->hasArg("assetTag")){
      status.assetTag = request->arg("assetTag");
    } else {
      request->send(400, "application/json", status.getStateJson());
    }
    storage.save(&status);
    status.current = currentSensor.getCurrentInAmps();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/lock", HTTP_GET, [](AsyncWebServerRequest *request) {
    lock();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/unlock", HTTP_GET, [](AsyncWebServerRequest *request) {
    status.cardId = "remote";
    unlock();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncElegantOTA.begin(&server);    // Start ElegantOTA
    status.mode = MODE_UPDATE;
    backoff.reset();
    request->send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"2; url=/update\" /><H1>Waiting 2 seconds for update server to start...</H1>");
  });
  
  server.begin();
}

void loop() {
  
  if(backoff.isReady()){
    switch(status.mode){
      case MODE_READ:
        readCard();
        break;
      case MODE_CALL_WEBHOOK:
        webhook();
        break;
      case MODE_UNLOCKED:
        if(currentSensorBackoff.isReady()){
          currentSensorBackoff.reset();
          currentSensorBackoff.setDelay();
          currentSensorBackoff.setDelay();
          status.current = currentSensor.getCurrentInAmps();
        }
        unlockLoop();
        break;
      case MODE_UPDATE:
        updateLoop();
        break;
    }
    
  }

}