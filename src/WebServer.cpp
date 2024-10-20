#include "WebServer.h"

AsyncWebServer webServer(80);

WebServer::WebServer() {}

void WebServer::handleDownloadLog(AsyncWebServerRequest *request)
{
  File file = LittleFS.open(LOG_FILE, "r");
  if (!file)
  {
    request->send(404, "text/plain", "File not found");
    return;
  }
  request->send(file, LOG_FILE, "application/octet-stream");
}

void WebServer::handleTransmit(AsyncWebServerRequest *request)
{
  if (request->getParam(PARAM_REPEAT))
  {
    int repeat = request->getParam(PARAM_REPEAT)->value().toInt();
    Radio::getInstance().setRepeatTransmit(repeat);
  }

  int length = -1;
  if (request->hasParam(PARAM_LENGTH))
  {
    length = request->getParam(PARAM_LENGTH)->value().toInt();
  }

  if (request->hasParam(PARAM_CODE) && request->hasParam(PARAM_PROTOCOL))
  {
    String code = request->getParam(PARAM_CODE)->value();
    String protocol = request->getParam(PARAM_PROTOCOL)->value();
    Radio::getInstance().sendCode(code, length, protocol.toInt());
    request->send(200, "text/javascript", "OK");
  }
}

void WebServer::handleGetSettings(AsyncWebServerRequest *request)
{
  double selectedFrequency = preferences.getDouble(FREQUENCY_PREFERENCE_NAME, defaultFrequency);
  int selectedBandwidth = preferences.getInt(PARAM_BANDWIDHT_NAME, defaultBandwidth);

  StaticJsonDocument<1024> doc;
  for (size_t i = 0; i < sizeof(FREQUENCIES) / sizeof(FREQUENCIES[0]); i++)
  {
    doc[i] = FREQUENCIES[i];
  }

  String response;
  DynamicJsonDocument responseDoc(1024);
  responseDoc["frequencies"] = doc.as<JsonArray>();
  responseDoc["frequency"] = selectedFrequency;
  responseDoc["bandwidth"] = selectedBandwidth;
  responseDoc["ssid"] = preferences.getString(WIFI_SSID_PREFERENCE_NAME, DEFAULT_WIFI_SSID);
  responseDoc["password"] = preferences.getString(WIFI_PASSWORD_PREFERENCE_NAME, DEFAULT_WIFI_PASSWORD);

  serializeJson(responseDoc, response);
  request->send(200, "application/json", response);
}

void WebServer::handleGetProtocols(AsyncWebServerRequest *request)
{
  DynamicJsonDocument responseDoc(256);
  JsonArray protocols = responseDoc.to<JsonArray>();
  for (size_t i = 0; i < PROTOCOL_COUNT; i++)
  {
    protocols.add(Radio::getInstance().protocolToString(static_cast<Protocol>(i)));
  }
  String response;
  serializeJson(responseDoc, response);
  request->send(200, "application/json", response);
}

void WebServer::handleServeStaticPage(AsyncWebServerRequest *request, String page)
{
  request->send(LittleFS, "/" + page, "text/html");
}

void WebServer::handleSettings(AsyncWebServerRequest *request)
{
  AsyncWebParameter *frequencyParam = request->getParam("frequency");
  AsyncWebParameter *bandwidthParam = request->getParam("bandwidth");
  AsyncWebParameter *ssidParam = request->getParam("ssid");
  AsyncWebParameter *passwordParam = request->getParam("password");

  if (frequencyParam != NULL)
  {
    String frequency = frequencyParam->value();
    preferences.putString(FREQUENCY_PREFERENCE_NAME, frequency);
    INFOF("Set frequency to %s\n", frequency.c_str());
  }

  if (bandwidthParam != NULL)
  {
    String bandwidth = bandwidthParam->value();
    preferences.putString(PARAM_BANDWIDHT_NAME, bandwidth);
    INFOF("Set bandwidth to %s\n", bandwidth.c_str());
  }

  if (ssidParam != NULL)
  {
    String ssid = ssidParam->value();
    preferences.putString(WIFI_SSID_PREFERENCE_NAME, ssid);
    INFO("ssid sucessfully changed");
  }

  if (passwordParam != NULL)
  {
    String password = passwordParam->value();
    preferences.putString(WIFI_PASSWORD_PREFERENCE_NAME, password);
    INFO("Wifi password sucessfully changed");
  }

  handleServeStaticPage(request, "settings.html");
}

void WebServer::handleWebRequest(AsyncWebServerRequest *request)
{
  if (request->method() != HTTP_GET)
  {
    request->send(400, "text/plain", "Invalid request");
    return;
  }

  String url = request->url();

  if (url.equals("/download"))
  {
    handleDownloadLog(request);
  }
  else if (url.equals("/transmit"))
  {
    handleTransmit(request);
  }
  else if (url.equals("/get-settings"))
  {
    handleGetSettings(request);
  }
  else if (url.equals("/settings"))
  {
    handleSettings(request);
  }
  else if (url.equals("/protocols"))
  {
    handleGetProtocols(request);
  }
  else if (url.equals("/delete"))
  {
    FileManager::getInstance().deleteFile(LOG_FILE);
    request->send(200, "text/plain", "OK");
  }
}

void WebServer::startWebServer()
{
  webServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=3600");
  for (const char *route : ROUTES)
  {
    webServer.on(route, HTTP_GET, [&](AsyncWebServerRequest *request)
                 { handleWebRequest(request); });
  }
  webServer.onNotFound([](AsyncWebServerRequest *request)
                       { request->send(404, "text/plain", "File not found"); });
  webServer.begin();
  INFO("HTTP server started");
}