class Events
{
public:
    static Events &getInstance()
    {
        static Events instance;
        return instance;
    }

    void handleEvent();
    void startTimerDisableAP(long timeout);
    void stopTimerDisableAP();
    void startTimerLedBlink(long timeout);
    void stopTimerLedBlink();

private:
    Events();
};