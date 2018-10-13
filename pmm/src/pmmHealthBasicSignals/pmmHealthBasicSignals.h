/* pmmHealthSignals.h
 * Code for reporting the system health via visual and sonorous signals,
 * Aka buzzers and Leds!*
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_HEALTH_SIGNALS_h
#define PMM_HEALTH_SIGNALS_h

#include <pmmConsts.h>
#include <pmmErrorsCentral.h>


class PmmHealthSignals
{
private:
    int mActualNumberOfErrors; // Total errors in the system number
    unsigned long mMillisNextSignalState; // Used updateLedsAndBuzzer
    uint8_t mSystemWasOk, mIsShortBeepOfSystemWasOk, mSignalIsOn, mSignalStarterCounter, mSignalActualErrorIndex, mSignalActualErrorCounter;
    PmmErrorsCentral* mPmmErrorsCentral;
    const pmmErrorStructType* mErrorsStructArray;

public:
    PmmHealthSignals();
    int init(PmmErrorsCentral* pmmErrorsCentral);
    int update();
    void blinkRfLED(int state);


};

#endif
