#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 

const char *host = "http://arduinojson.org/example.json";

void updateRepository(){
  HTTPClient http;
  http.begin(host);
  int httpCode = http.GET();
  String payload = http.getString();
  setMessage(payload);
  delay(3000);
  setMessage("RFID Lock");
}
