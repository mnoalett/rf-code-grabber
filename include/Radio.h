#ifndef radio_h
#define radio_h

#include <RCSwitch.h>
#include <ELECHOUSE_CC1101.h>
#include "Logging.h"

typedef struct
{
    unsigned long decimalValue;
    unsigned int length;
    unsigned int delay;
    unsigned int protocol;
    unsigned int *raw;
    char *bytes;
} Signal;

static RCSwitch rcSwitch = RCSwitch();
int rxPin;
int txPin;

void initializeRadio()
{
#ifdef ESP32
    rxPin = 4;
    txPin = 0;
#elif ESP8266
    rxPin = D1;
    txPin = D2;
#else
    rxPin = 0; // for Arduino! Receiver on interrupt 0 => that is pin #2
#endif

    if (!ELECHOUSE_cc1101.getCC1101())
    {
        ERROR("SPI connection error");
    }
    else
    {
        INFO("SPI connection OK");
    }

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(433.92);
    ELECHOUSE_cc1101.SetRx();
    rcSwitch.setRepeatTransmit(15);
    rcSwitch.enableReceive(rxPin);
}

void sendCode(String code, int protocol)
{
    rcSwitch.disableReceive();
    rcSwitch.enableTransmit(txPin);
    ELECHOUSE_cc1101.SetTx();

    INFOF("Sending %s\n\t\t\tProtocol %d", code.c_str(), protocol);

    rcSwitch.setProtocol(protocol);
    rcSwitch.send(code.c_str());
    delay(1000);

    rcSwitch.disableTransmit();
    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.SetTx();
    ELECHOUSE_cc1101.SetRx();
    rcSwitch.enableReceive(rxPin);

    rcSwitch.resetAvailable();
}

#endif
