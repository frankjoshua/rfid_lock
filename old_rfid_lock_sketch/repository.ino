#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include <ArduinoJson.h>

//const char *host = "http://arduinojson.org/example.json";
const char *host = "http://archreactor.org/sites/all/modules/civicrm/extern/rest.php\
?entity=Contact&action=get\
&api_key=%s&key=%s\
&json={\"sequential\":1,\"return\":\"display_name,custom_7,custom_12\",\"options\":{\"limit\":1}}&custom_7=%s";

DynamicJsonDocument doc(1024);
char input[1024];

void updateRepository(String apiKey, String key, String rfid){
  setMessage("Key Lookup");
  HTTPClient http;
  sprintf(input, host, apiKey.c_str(), key.c_str(), rfid.c_str());
  Serial.println(input);
  http.begin(input);
  int httpCode = http.GET();
  String json = http.getString();
  deserializeJson(doc, json);
  if(!doc["error_message"].isNull()){
    setMessage(doc["error_message"]);
  } else if(doc["count"] == 1){
    setMessage("Retry");
    authCardId = rfid;
  } else {
    if(doc["count"].isNull()){
      setMessage("Denied!!!");
    } else {
      setMessage(doc["count"]);
    }
  }
}
