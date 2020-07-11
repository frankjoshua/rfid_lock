#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define MODE_READ 0
#define MODE_CALL_WEBHOOK 1
#define MODE_UNLOCKED 2
#define MODE_UPDATE 3

class Status {
  public:
    String cardId = "";
    int mode = MODE_READ;
    String assetTag = "01001";
    String msg = "";
    String getStateJson();
};

inline String Status::getStateJson() {
  return "{\"mode\": \"" + String(mode) +  "\",\"assetTag\": \"" + assetTag + "\"}";
}

#endif