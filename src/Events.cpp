#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Events.h"
#include "Logging.h"

static long timeoutDisableAP;
static long timeoutLedBlink;
static long startTimeoutDisableAP;
static bool enabledTimerDisableAP;
static long startTimeoutLedBlink;
static bool enabledTimerLedBlink;

Events::Events() {}

void Events::startTimerDisableAP(long timeout)
{
    timeoutDisableAP = timeout;
    startTimeoutDisableAP = millis();
    enabledTimerDisableAP = true;
}

void Events::stopTimerDisableAP()
{
    enabledTimerDisableAP = false;
}

void Events::startTimerLedBlink(long timeout)
{
    timeoutLedBlink = timeout;
    startTimeoutLedBlink = millis();
    enabledTimerLedBlink = true;
}

void Events::stopTimerLedBlink()
{
    enabledTimerLedBlink = false;
}

void Events::handleEvent()
{
    if (enabledTimerDisableAP && ((millis() - startTimeoutDisableAP) > timeoutDisableAP))
    {
        enabledTimerDisableAP = false;
        if (!wifi_softap_get_station_num())
        {
            WiFi.softAPdisconnect(true);
            INFO("AP deactivated. Entering Light Sleep");
            stopTimerLedBlink();
            digitalWrite(LED_BUILTIN, HIGH);
            wifi_set_sleep_type(LIGHT_SLEEP_T);
        }
        else
        {
            Events::startTimerDisableAP(10000);
        }
    }

    if (enabledTimerLedBlink && ((millis() - startTimeoutLedBlink) > timeoutLedBlink))
    {
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
        startTimerLedBlink(timeoutLedBlink);
    }
}
