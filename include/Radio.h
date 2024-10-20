#ifndef radio_h
#define radio_h

#include <RCSwitch.h>
#include <ELECHOUSE_CC1101.h>
#include <Preferences.h>
#include "Logging.h"

constexpr auto AM270 = 256;
constexpr auto AM650 = 650;
constexpr auto AM812 = 812;
constexpr int BANDWIDTHS[] = {AM270, AM650, AM812};
constexpr auto FREQUENCY_PREFERENCE_NAME = "frequency";
constexpr auto PARAM_BANDWIDHT_NAME = "bandwidth";
constexpr double FREQUENCIES[] = {
    300.0, 310.0, 315.0, 315.1, 315.4, 315.8, 318.0, 390.0, 433.0, 433.075, 433.330,
    433.650, 433.92, 433.94, 868.025, 868.3, 868.35, 868.7, 915.0, 915.025, 915.2, 915.5};

extern Preferences preferences;
extern RCSwitch rcSwitch;
extern double defaultFrequency;
extern int defaultBandwidth;

enum Protocol
{
    CAME,
    FAAC,
    PRINCETON,
    NICE_FLO,
    NICE_FLOR_S,
    PROTOCOL_COUNT
};

class Radio
{
public:
    static Radio &getInstance()
    {
        static Radio instance;
        return instance;
    }

    void initialize();
    void setFrequency(double frequency);
    void setBandwidth(int bandwidth);
    void setRepeatTransmit(int nRepeat);
    void sendCode(String code, int protocol);
    void sendCode(String code, int len, int protocol);
    String protocolToString(Protocol p);

private:
    Radio();
};

#endif