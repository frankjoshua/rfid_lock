#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h> 

ESP8266WebServer server(80);
String api_key = "";
String user_key = "";

void webServer(){
  server.send(200, "text/html", "api_key = " + api_key + ", user_key = " + user_key);
}

void configPage(){
  api_key = server.arg("api_key");
  user_key = server.arg("user_key");
  webServer();
}

void initFiles(){
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");

      } else {
        Serial.println("failed to load json config");
      }
        configFile.close();
      }
  } else {
    Serial.println("failed to mount FS");
  }

  server.on("/", webServer);
  server.on("/config", configPage);
  server.begin();
}

void serverLoop() {
  server.handleClient();
}

String getApiKey() {
  return api_key;
}

String getUserKey() {
  return user_key;
}
