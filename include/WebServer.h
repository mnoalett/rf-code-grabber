#ifndef webserver_h
#define webserver_h

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "FileManager.h"
#include "Logging.h"
#include "Radio.h"
#include "Config.h"

constexpr const char *ROUTES[] = {
    "/download",
    "/execute",
    "/delete",
    "/get-settings",
    "/transmit",
    "/settings"
    "/protocols"};

constexpr const char *PARAM_CODE = "code";
constexpr const char *PARAM_PROTOCOL = "protocol";
constexpr const char *PARAM_REPEAT = "repeat";
constexpr const char *PARAM_LENGTH = "length";

extern AsyncWebServer webServer;

class WebServer
{
public:
    static WebServer &getInstance()
    {
        static WebServer instance;
        return instance;
    }
    void startWebServer();

private:
    WebServer();
    void handleWebRequest(AsyncWebServerRequest *request);
    void handleTransmit(AsyncWebServerRequest *request);
    void handleDownloadLog(AsyncWebServerRequest *request);
    void handleGetSettings(AsyncWebServerRequest *request);
    void handleServeStaticPage(AsyncWebServerRequest *request, String page);
    void handleSettings(AsyncWebServerRequest *request);
    void handleGetProtocols(AsyncWebServerRequest *request);
};

#endif