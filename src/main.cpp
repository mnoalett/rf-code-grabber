#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <Time.h>
#include "Config.h"
#include "Output.h"
#include "FileManager.h"
#include "LittleFS.h"
#include "Events.h"
#include "Logging.h"
#include "Radio.h"
#include "WebServer.h"

const char DEFAULT_WIFI_SSID[] = "ESP-Net";
const char DEFAULT_WIFI_PASSWORD[] = "1234567890";
const long TIMEOUT_SAME_RX = 1000;

IPAddress localIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

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
  Radio::getInstance().initialize();

  WiFi.mode(WIFI_AP);
  WiFi.onEvent(wiFiEventHandler);
  WiFi.softAPConfig(localIP, gateway, subnet);
  WiFi.softAP(preferences.getString(WIFI_SSID_PREFERENCE_NAME, DEFAULT_WIFI_SSID), preferences.getString(WIFI_PASSWORD_PREFERENCE_NAME, DEFAULT_WIFI_PASSWORD));

  event.startTimerDisableAP(TIMEOUT_DISABLE_ACCESS_POINT);

  INFOF("Access Point started. Waiting %d minute(s) before deactivating\n\t\t\tIP: %s", TIMEOUT_DISABLE_ACCESS_POINT / 60000, localIP.toString().c_str());
  WebServer::getInstance().startWebServer();
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

    String signalPayload = toJsonString(decimal, length, delay, b, protocol, raw);

    fileManager.appendFile(LOG_FILE, signalPayload.c_str());
    fileManager.appendFile(LOG_FILE, "\n");

    rcSwitch.resetAvailable();
  }
}