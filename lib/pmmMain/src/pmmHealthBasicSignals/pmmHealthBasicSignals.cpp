/* pmmHealthSignals.h
 * Code for reporting the system health via visual and sonorous signals,
 * Aka buzzers and Leds!*
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <Arduino.h> // For PROGMEM. Also includes uint32_t type, so no stdint.h include needed.

#include <pmmConsts.h>
#include "pmmHealthBasicSignals/pmmHealthBasicSignals.h"

PmmHealthSignals::PmmHealthSignals()
{
}

// Initializer
int PmmHealthSignals::init()
{
    mMillisNextSignalState = 0;

    pinMode(PMM_PIN_LED_RECOVERY, OUTPUT);
    pinMode(PMM_PIN_LED_ERRORS, OUTPUT);
    pinMode(PMM_PIN_ALL_OK_AND_TELEMETRY, OUTPUT);
    pinMode(PMM_PIN_BUZZER,OUTPUT);

    digitalWrite(PMM_PIN_LED_RECOVERY, LOW);
    digitalWrite(PMM_PIN_LED_ERRORS, LOW);
    digitalWrite(PMM_PIN_ALL_OK_AND_TELEMETRY, HIGH); // Initializes it HIGH.
    digitalWrite(PMM_PIN_BUZZER, LOW);
    mSystemWasOk = mIsShortBeepOfSystemWasOk = 1; mSignalIsOn = mSignalStarterCounter = mSignalActualErrorIndex = mSignalActualErrorCounter = 0;
    return 0;
}


//       ..........     ..........
//  .....          .....          ..... 0xFF 0xFF 0xii 0xii 0xii
// A confusing but working code.
int PmmHealthSignals::update()
{
    if (millis() >= mMillisNextSignalState)
    {
        if (mSystemWasOk)
        {
            if (!mSignalIsOn) // If signal not On
            {
                mSignalIsOn = 1;
                #if BUZZER_ACTIVATED
                    digitalWrite(PMM_PIN_BUZZER, HIGH); // Turn On
                #endif
                if (mIsShortBeepOfSystemWasOk)
                {
                    mMillisNextSignalState = millis() + 100; // Long beep On
                }
                else
                {
                    mMillisNextSignalState = millis() + 1000; // Long beep On
                }
            }
            else // So signal is On
            {
                mSignalIsOn = 0;
                #if BUZZER_ACTIVATED
                    digitalWrite(PMM_PIN_BUZZER, LOW); // Turn Off
                #endif
                if (mIsShortBeepOfSystemWasOk)
                {
                    mMillisNextSignalState = millis() + 100; // Short beep Off
                    mIsShortBeepOfSystemWasOk = 0;
                }
                else
                {
                    mIsShortBeepOfSystemWasOk = 1;
                    mMillisNextSignalState = millis() + 1000; // Long beep Off
                    if (mActualNumberOfErrors)
                        mSystemWasOk = 0; // So the signal will have a low state of >500ms before the error signal
                }
            }

        }
        else // System has an error
        {
            if (!mSignalIsOn) // If signal not On
            {
                mSignalIsOn = 1;

                #if BUZZER_ACTIVATED
                    digitalWrite(PMM_PIN_BUZZER, HIGH); // Turn On
                #endif
                digitalWrite(PMM_PIN_LED_ERRORS, HIGH);

                if (!mSignalActualErrorCounter) // If is a header beep
                {
                    if (!mSignalStarterCounter) // mSignalStarterCounter needed to don't keep assigning the value
                    {
                        if (mSignalActualErrorIndex == 0)
                        {
                            mSignalStarterCounter = 3; // The first error has 3 short signals before the error code
                        }
                        else
                        {
                            mSignalStarterCounter = 2; // The subsequent errors has 2 short beeps before the error code
                        }
                    }
                    mMillisNextSignalState = millis() + 100; // Short Beep High - Make small signals to show the start of an error.
                }
                else // Is an error code beep
                    mMillisNextSignalState = millis() + 300; // Medium Beep Low
            }
            else // So signal is On
            {
                mSignalIsOn = 0;
                #if BUZZER_ACTIVATED
                    digitalWrite(PMM_PIN_BUZZER, LOW); // Turn Off
                #endif
                digitalWrite(PMM_PIN_LED_ERRORS, LOW);
                if (mSignalStarterCounter) // Is a header beep
                {
                    if (--mSignalStarterCounter > 0)
                    {
                        mMillisNextSignalState = millis() + 100; // Short Beep Low
                    }
                    else // If the signal starter is now 0, start the error code signal.
                    {
                        //mSignalActualErrorCounter = mErrorsStructArray[mSignalActualErrorIndex].code; // Short beeps are over, load the next error
                        mMillisNextSignalState = millis() + 500;
                    }
                }
                else //Is an error beep
                {
                    if (--mSignalActualErrorCounter > 0) // If is a low level between errors beeps
                        mMillisNextSignalState = millis() + 500; // Medium Beep Low
                    else // Is a low lever after all the errors beeps, get the next error ID (go to the first one again)
                    {
                        mMillisNextSignalState = millis() + 1000; // Medium Beep Low
                        if ((++mSignalActualErrorIndex) >= mActualNumberOfErrors) // If the index++ is bigger than the maximum index value
                            mSignalActualErrorIndex = 0;
                    }
                }
            }
        }
    }
    return 0;
}


void PmmHealthSignals::blinkRfLED(int state)
{
    if (!mActualNumberOfErrors)
        digitalWrite(PMM_PIN_ALL_OK_AND_TELEMETRY, state);
}
