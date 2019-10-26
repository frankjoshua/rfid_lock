#include <DNSServer.h>
#include <ESP8266WebServer.h> 
#include <WiFiManager.h>

void initWifi(String apName){
  WiFiManager wifiManager;
  wifiManager.autoConnect(apName.c_str());
}
