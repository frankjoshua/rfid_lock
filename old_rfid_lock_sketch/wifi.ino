#include <DNSServer.h>
#include <ESP8266WebServer.h> 
#include <WiFiManager.h>

WiFiManager wifiManager;

void initWifi(){
  wifiManager.autoConnect();
}
