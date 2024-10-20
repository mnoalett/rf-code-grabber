#include "Radio.h"

RCSwitch rcSwitch = RCSwitch();

int rxPin;
int txPin;
double defaultFrequency = 433.92;
int defaultBandwidth = AM812;
double usedFrequency;
int usedBandwidth;
int defaultRepeatTransmit = 3;

Preferences preferences;

Radio::Radio() {}

void Radio::initialize()
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

    preferences.begin("settings");
    usedFrequency = preferences.getDouble("frequency", defaultFrequency);
    usedBandwidth = preferences.getDouble("bandwidth", defaultBandwidth);

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(usedFrequency);
    ELECHOUSE_cc1101.SetRx();
    rcSwitch.setRepeatTransmit(defaultRepeatTransmit);
    rcSwitch.enableReceive(rxPin);
}

void Radio::setFrequency(double frequency)
{
    preferences.putDouble("frequency", frequency);
    ELECHOUSE_cc1101.setMHZ(frequency);
}

void Radio::setBandwidth(int bandwidth)
{
    preferences.putInt("bandwidth", bandwidth);
    ELECHOUSE_cc1101.setRxBW(bandwidth);
}

void Radio::setRepeatTransmit(int nRepeat)
{
    rcSwitch.setRepeatTransmit(nRepeat);
}

void Radio::sendCode(String code, int protocol)
{
    sendCode(code, -1, protocol);
}

void Radio::sendCode(String code, int len, int protocol)
{
    rcSwitch.disableReceive();
    rcSwitch.enableTransmit(txPin);
    ELECHOUSE_cc1101.SetTx();

    INFOF("Sending %s\n\t\t\tProtocol %d\n\t\t\tLen %d", code.c_str(), protocol, len);

    rcSwitch.setProtocol(protocol);

    if (len == -1)
    {
        rcSwitch.send(code.c_str());
    }
    else
    {
        unsigned long long value = code.toInt();
        rcSwitch.send(value, len);
    }
    delay(1000);

    rcSwitch.disableTransmit();
    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.SetTx();
    ELECHOUSE_cc1101.SetRx();
    rcSwitch.enableReceive(rxPin);

    rcSwitch.resetAvailable();
}

String Radio::protocolToString(Protocol protocol)
{
    switch (protocol)
    {
    case CAME:
        return "Came";
    case FAAC:
        return "Faac";
    case PRINCETON:
        return "Princeton";
    case NICE_FLO:
        return "Nice flo";
    case NICE_FLOR_S:
        return "Nice Floor S";
    default:
        return "Unknown protocol";
    }
}
