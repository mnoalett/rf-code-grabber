#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Output.h"
#include "FileManager.h"
#include "LittleFS.h"
#include "Events.h"
#include "Logging.h"
#include "Radio.h"

const char LOG_FILE[] = "/capture.txt";
const char SSID[] = "ESP-Net";
const char PASSWORD[] = "Password1";
const long TIMEOUT_DISABLE_ACCESS_POINT = 60 * 1000;
const long TIMER_LED_BLINK_500MS = 500;
const char *PARAM_CODE = "code";
const char *PARAM_PROTOCOL = "protocol";

IPAddress localIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

FileManager &fileManager = FileManager::getInstance();
Events &event = Events::getInstance();

void wiFiEventHandler(WiFiEvent_t event)
{
  switch (event)
  {
  case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
    INFO("Client connected");
    break;
  case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
    INFO("Client disconnected");
    break;
  }
}

void handleWebRequest(AsyncWebServerRequest *request)
{
  if (request->method() == HTTP_GET)
  {
    if (request->url().equals("/download"))
    {
      File file = LittleFS.open(LOG_FILE, "r");
      if (!file)
      {
        request->send(404, "text/plain", "File not found");
        return;
      }
      request->send(file, LOG_FILE, "application/octet-stream");
    }
    else if (request->url().equals("/execute"))
    {
      if (request->hasParam(PARAM_CODE) && request->hasParam(PARAM_PROTOCOL))
      {
        String code = request->getParam(PARAM_CODE)->value();
        String protocol = request->getParam(PARAM_PROTOCOL)->value();
        sendCode(code, protocol.toInt());
        request->send(200, "text/plain", "OK");
      }
    }
    else if (request->url().equals("/delete"))
    {
      fileManager.deleteFile(LOG_FILE);
      request->send(200, "text/plain", "OK");
    }
    else
    {
      request->send(400, "text/plain", "Invalid request");
    }
  }
}

void startWebServer()
{
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=3600");

  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request)
            { handleWebRequest(request); });
  server.on("/execute", HTTP_GET, [](AsyncWebServerRequest *request)
            { handleWebRequest(request); });
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
            { handleWebRequest(request); });
  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/command.html", "text/html"); });
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "File not found"); });
  server.begin();
  INFO("HTTP server started");
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  event.startTimerLedBlink(TIMER_LED_BLINK_500MS);

  if (!LittleFS.begin())
  {
    ERROR("Failed to mount LittleFS");
  }

  fileManager.createFile(LOG_FILE);

  initializeRadio();

  WiFi.mode(WIFI_AP);
  WiFi.onEvent(wiFiEventHandler);
  WiFi.softAPConfig(localIP, gateway, subnet);
  WiFi.softAP(SSID, PASSWORD);

  event.startTimerDisableAP(TIMEOUT_DISABLE_ACCESS_POINT);

  INFOF("Access Point started. Waiting %d minute(s) before deactivating\n\t\t\tIP: %s", TIMEOUT_DISABLE_ACCESS_POINT / 60000, localIP.toString().c_str());
  startWebServer();
}

void loop()
{
  event.handleEvent();

  if (rcSwitch.available())
  {
    unsigned long decimal = rcSwitch.getReceivedValue();
    unsigned int length = rcSwitch.getReceivedBitlength();
    unsigned int delay = rcSwitch.getReceivedDelay();
    unsigned int *raw = rcSwitch.getReceivedRawdata();
    unsigned int protocol = rcSwitch.getReceivedProtocol();
    char *b = dec2binWzerofill(decimal, length);

    String signalPayload = toJsonString(decimal, length, delay, b, protocol);

    fileManager.appendFile(LOG_FILE, signalPayload.c_str());
    fileManager.appendFile(LOG_FILE, "\n");

    output(decimal, length, delay, raw, protocol);
    rcSwitch.resetAvailable();
  }
}