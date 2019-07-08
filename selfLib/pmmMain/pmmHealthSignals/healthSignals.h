/* pmmHealthSignals.h
 * Code for reporting the system health via visual and sonorous signals,
 * Aka buzzers and Leds!*
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_HEALTH_SIGNALS_h
#define PMM_HEALTH_SIGNALS_h

#include <pmmConsts.h>


class PmmHealthSignals
{

public:

    PmmHealthSignals();
    int init();
    int update();
    void blinkRfLED(int state);

private:

    int           mActualNumberOfErrors; // Total errors in the system number
    unsigned long mMillisNextSignalState; // Used updateLedsAndBuzzer
    uint8_t       mSystemWasOk, mIsShortBeepOfSystemWasOk, mSignalIsOn, mSignalStarterCounter, mSignalActualErrorIndex, mSignalActualErrorCounter;

};

#endif
