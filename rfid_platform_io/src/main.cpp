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
AsyncWiFiManager wifiManager(&server, &dns);
RfidReader rfidReader;
BackoffTimer backoff;
BackoffTimer currentSensorBackoff;
Display display;
CurrentSensor currentSensor;
Status status;
Storage storage;

#define RELAY_PIN D7
#define BELL_PIN D3

const String TOOL_CHECKOUT_URL = "https://api.archreactor.net";

unsigned long lockAtTime = 0;

void relayOn()
{
  digitalWrite(RELAY_PIN, HIGH);
}

void relayOff()
{
  digitalWrite(RELAY_PIN, LOW);
}

void bellOn()
{
  digitalWrite(BELL_PIN, HIGH);
}

void bellOff()
{
  digitalWrite(BELL_PIN, LOW);
}

void displayMessage(String msg)
{
  status.msg = msg;
  display.print(&status);
  Serial.println(msg);
}

void readCard()
{
  if (millis() % 1000 == 0)
  {
    displayMessage("");
  }
  if (rfidReader.isCardAvailable())
  {
    status.cardId = rfidReader.getCardId();
    if (status.cardId.length() <= 2)
    {
      if (status.cardId == "d")
      {
        status.mode = MODE_RING_BELL;
      }
    }
    else
    {
      status.mode = MODE_CALL_WEBHOOK;
    }
    backoff.reset();
    displayMessage("Please wait");
  }
}

void lock()
{
  relayOff();
  status.mode = MODE_READ;
}

void unlock()
{
  status.mode = MODE_UNLOCKED;
  lockAtTime = millis() + status.unlockDuration;
  backoff.reset();
}

void webhook()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    displayMessage("No WIFI!");
    backoff.setDelay();
    return;
  }

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure(); //the magic line, use with caution
  String checkoutUrl = TOOL_CHECKOUT_URL + "/tool/" + status.assetTag + "/checkout/" + status.cardId;
  bool httpInitResult = http.begin(client, checkoutUrl);
  if (!httpInitResult)
  {
    displayMessage("Could not init http!");
    backoff.setDelay();
    return;
  }

  int httpCode = http.GET();
  char error[256];
  switch (httpCode)
  {
  case 404: // User not found
  case 401: // User not authorized
  case 500: // Server Error Unknown
    displayMessage("Denied: " + String(httpCode));
    status.mode = MODE_READ;
    backoff.setDelay();
    backoff.setDelay();
    status.error = "Denied: " + String(httpCode) + " " + checkoutUrl;
    break;
  case 200:
    status.error = "";
    unlock();
    break;
  case 301: // http redirect still unlock
  case -1:  // Connection Refused - Server Down - Still unlock
  default:
    sprintf(error, "%s\n%d", http.errorToString(httpCode).c_str(), httpCode);
    status.error = error;
    unlock();
    break;
  }

  http.end();
  backoff.setDelay();
}

void webhookCheckIn()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    displayMessage("No WIFI!");
    backoff.setDelay();
    return;
  }

  WiFiClientSecure client;
  client.setInsecure(); //the magic line, use with caution
  HTTPClient http;
  bool httpInitResult = http.begin(client, TOOL_CHECKOUT_URL + "/tool/" + status.assetTag + "/checkin");
  if (!httpInitResult)
  {
    displayMessage("Could not init http!");
    backoff.setDelay();
    return;
  }

  int httpCode = http.GET();
  char error[256];
  switch (httpCode)
  {
  case 200: // OK
    lock();
    break;
  case 404: // User not found
  case 401: // User not authorized
  case 500: // Server Error Unknown
  case 301: // Http redirect
  case -1:  // Connection Refused
  default:
    sprintf(error, "%s\n%d", http.errorToString(httpCode).c_str(), httpCode);
    status.error = error;
    lock();
    break;
  }

  http.end();
  backoff.setDelay();
}

String timeToString(unsigned long t)
{
  static char str[5];
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;
  sprintf(str, "%01d:%02d", m, s);
  return String(str);
}

void unlockLoop()
{
  backoff.reset();
  backoff.setDelay();
  if (status.current > 0.2)
  {
    lockAtTime = millis() + status.unlockDuration;
  }
  int timeLeft = (lockAtTime - millis()) / 1000;
  if (timeLeft > 0)
  {
    relayOn();
    displayMessage(timeToString(timeLeft) + " " + String(status.current) + "A");
  }
  else
  {
    status.mode = MODE_CALL_WEBHOOK_DISCONNECT;
  }
}

void updateLoop()
{
  unsigned long time = millis();
  if (time % 1500 == 0)
  {
    displayMessage("......");
  }
  else if (time % 1000 == 0)
  {
    displayMessage("....");
  }
  else if (time % 500 == 0)
  {
    displayMessage(".");
  }
  AsyncElegantOTA.loop();
}

void ringBellLoop()
{
  for (int i = 0; i < 5; i++)
  {
    bellOn();
    delay(150);
    bellOff();
    delay(50);
  }
  status.mode = MODE_READ;
}

/*
* True is Request has correct Key or if Key is not required
*/
bool hasValidKey(AsyncWebServerRequest *request)
{
  if (status.secretKey != "")
  {
    return request->hasArg("key") && status.secretKey == request->arg("key");
  }
  return true;
}

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BELL_PIN, OUTPUT);
  relayOff();
  bellOff();

  Serial.begin(115200);
  while (!Serial)
  {
  }
  delay(2000);

  storage.restore(&status);
  Serial.println("Initalizing display.");
  display.initDisplay();
  displayMessage("Wifi <<*>>");
  wifiManager.autoConnect("ArchReactorLockout");
  displayMessage("");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    status.current = currentSensor.getCurrentInAmps();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasArg("key"))
    {
      if (status.secretKey == "" || status.secretKey == request->arg("key"))
      {
        if (request->hasArg("assetTag"))
        {
          status.assetTag = request->arg("assetTag");
        }
        if (request->hasArg("newKey"))
        {
          status.secretKey = request->arg("newKey");
        }
        if (request->hasArg("unlockDuration"))
        {
          status.unlockDuration = request->arg("unlockDuration").toInt();
        }
      }
      else
      {
        request->send(401, "application/json", status.getStateJson());
        return;
      }
    }
    else
    {
      request->send(400, "application/json", status.getStateJson());
      return;
    }
    storage.save(&status);
    status.current = currentSensor.getCurrentInAmps();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/lock", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Check for valid key
    if (!hasValidKey(request))
    {
      status.error = "401 Bad Key /lock";
      request->send(401, "application/json", status.getStateJson());
      return;
    }
    lock();
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/unlock", HTTP_GET, [](AsyncWebServerRequest *request) {
    status.cardId = "remote";
    // Check for valid key
    if (!hasValidKey(request))
    {
      status.error = "401 Bad Key /unlock";
      request->send(401, "application/json", status.getStateJson());
      return;
    }
    if (request->hasArg("cardId"))
    {
      status.cardId = request->arg("cardId");
      status.mode = MODE_CALL_WEBHOOK;
    }
    else
    {
      unlock();
    }
    request->send(200, "application/json", status.getStateJson());
  });

  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Check for valid key
    if (!hasValidKey(request))
    {
      status.error = "401 Bad Key /upload";
      request->send(401, "application/json", status.getStateJson());
      return;
    }
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    status.mode = MODE_UPDATE;
    backoff.reset();
    request->send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"2; url=/update\" /><H1>Waiting 2 seconds for update server to start...</H1>");
  });

  server.begin();

  // Ready to read cards
  status.mode = MODE_READ;
  Serial.println(WiFi.dnsIP());
  IPAddress result;
  int err = WiFi.hostByName("api.archreactor.net", result);
  if (err == 1)
  {
    Serial.print("Ip address: ");
    Serial.println(result);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(err);
  }
}

void loop()
{

  if (backoff.isReady())
  {
    switch (status.mode)
    {
    case MODE_READ:
      readCard();
      break;
    case MODE_CALL_WEBHOOK:
      webhook();
      break;
    case MODE_CALL_WEBHOOK_DISCONNECT:
      webhookCheckIn();
      break;
    case MODE_UNLOCKED:
      if (currentSensorBackoff.isReady())
      {
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
    case MODE_RING_BELL:
      ringBellLoop();
      break;
    }
  }
}