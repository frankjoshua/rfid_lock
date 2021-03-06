#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define DEFAULT_LOCK_DURATION 5 * 60 * 1000

#define MODE_STARTING -1
#define MODE_READ 0
#define MODE_CALL_WEBHOOK 1
#define MODE_UNLOCKED 2
#define MODE_UPDATE 3
#define MODE_CALL_WEBHOOK_DISCONNECT 4
#define MODE_RING_BELL 5

class Status
{
public:
  String cardId = "";
  int mode = MODE_STARTING;
  int unlockDuration = DEFAULT_LOCK_DURATION;
  String assetTag = "01001";
  String msg = "";
  String error = "";
  float current = 0;
  String headline = "";
  String secretKey = "";
  String getStateJson();
};

inline String Status::getStateJson()
{
  return "{\"mode\":\"" + String(mode) + "\",\"assetTag\":\"" + assetTag + "\",\"unlockDuration\":\"" + String(unlockDuration) + "\",\"current\":" + String(current) + ",\"headline\":\"" + headline + "\",\"msg\":\"" + msg + "\",\"cardId\":\"" + cardId + "\",\"error\":\"" + error + "\"}";
}

#endif