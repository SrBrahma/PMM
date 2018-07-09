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

    pmmErrorType mErrorsArray[ERRORS_ARRAY_SIZE]; // Total erros in the system
    int mActualNumberOfErrors; // Total errors in the system number
    unsigned long mMillisNextSignalState; // Used updateLedsAndBuzzer

public:
    PmmHealthSignals();
    int init(PmmErrorsCentral* pmmErrorsCentral);
    int update();
    void blinkRfLED(int state);


};

#endif
