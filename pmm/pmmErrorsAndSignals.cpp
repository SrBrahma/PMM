#include <pmmConsts.h>
#include <cstring>
#include <pmmTelemetry.h>
#include <pmmErrorsAndSignals.h>
#include <pmmSd.h>
/*
typedef enum {
    OK,
    ERROR_SD,
    ERROR_SD_WRITE,
    ERROR_RF_INIT,
    ERROR_RF_SET_FREQ,
    ERROR_ACCELEROMETER_INIT,
    ERROR_GYROSCOPE_INIT,
    ERROR_MAGNETOMETER_INIT,
    ERROR_BAROMETER_INIT,
    ERROR_PROGRAMMING,
    ERRORS_AMOUNT
} pmmErrorType;
*/
const char *pmmErrorString[ERRORS_AMOUNT] = {
    "No errors",                // OK,
    "SD init fail",             // ERROR_SD,
    "SD write fail",            // ERROR_SD_WRITE,
    "RF init fail",             // ERROR_RF_INIT,
    "RF Set Freq fail",         // ERROR_RF_SET_FREQ,
    "Accelerometer init fail",  // ERROR_ACCELEROMETER_INIT,
    "Gyroscope init fail",      // ERROR_GYROSCOPE_INIT,
    "Magnetometer init fail",   // ERROR_MAGNETOMETER_INIT,
    "Barometer init fail",      // ERROR_BAROMETER_INIT
    "Programming error"         // ERROR_PROGRAMMING
};
char recuperationActivatedString[] = "Recuperation Activated!";


PmmErrorsAndSignals::PmmErrorsAndSignals()
{
}

// Initializer
void PmmErrorsAndSignals::init(PmmSd *pmmSd, PmmTelemetry *pmmTelemetry, uint16_t fileID)
{
    // Init variables values
    mPmmSd = pmmSd;
    mPmmTelemetry = pmmTelemetry;
    mActualNumberOfErrors = 0;
    snprintf(mFilenameExtra, PMM_SD_FILENAME_MAX_LENGTH, "%s%03u%s", FILENAME_BASE_PREFIX, fileID, FILENAME_EXTRA_SUFFIX); // Declaration of the filename of the extra log

    void setFilename(char *mFilenameExtra);

    mSystemWasOk = mIsShortBeepOfSystemWasOk = 1; mSignalIsOn = mSignalStarterCounter = mSignalActualErrorIndex = mSignalActualErrorCounter = 0;
    mSdIsWorking = mGpsIsWorking = mTelemetryIsWorking = mBarometerIsWorking = mAccelerometerIsWorking = mGyroscopeIsWorking = mMagnetometerIsWorking = 1; // All systems starts functional
    mMillisNextSignalState = 0;

    pinMode(PMM_PIN_LED_RECOVERY, OUTPUT);
    pinMode(PMM_PIN_LED_ERRORS, OUTPUT);
    pinMode(PMM_PIN_ALL_OK_AND_TELEMETRY, OUTPUT);
    pinMode(PMM_PIN_BUZZER,OUTPUT);

    digitalWrite(PMM_PIN_LED_RECOVERY, LOW);
    digitalWrite(PMM_PIN_LED_ERRORS, LOW);
    digitalWrite(PMM_PIN_ALL_OK_AND_TELEMETRY, HIGH); // Initializes it HIGH.
    digitalWrite(PMM_PIN_BUZZER, LOW);
}


//       ..........     ..........
//  .....          .....          .....
//
void PmmErrorsAndSignals::updateLedsAndBuzzer()
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
                        mSignalActualErrorCounter = mErrorsArray[mSignalActualErrorIndex]; // Short beeps are over, load the next error
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
}

// [1090;763.312s Error 3: SD]
void PmmErrorsAndSignals::reportError(pmmErrorType errorId, unsigned long packageId)
{
    char errorString[ERROR_STRING_LENGTH];

    digitalWrite(PMM_PIN_ALL_OK_AND_TELEMETRY, LOW); // Make sure the All OK Led is Off (or turn it off if first time)

    if (errorId < 0 or errorId >= ERRORS_AMOUNT)
        errorId = ERROR_PROGRAMMING;

    snprintf(errorString, ERROR_STRING_LENGTH, "%s[%lu;%.3fs Error %i: %s]", RF_VALIDATION_HEADER_EXTRA, packageId, millis() / 1000.0, errorId, pmmErrorString[errorId]);

    if (mActualNumberOfErrors < ERRORS_ARRAY_SIZE)
        mErrorsArray[mActualNumberOfErrors++] = errorId;
    if (mSdIsWorking)
        mPmmSd->writeStringToFilename(mFilenameExtra, errorString + 4); // +4 to skip the MNEX header
    if (mTelemetryIsWorking)
        mPmmTelemetry->send((uint8_t*)errorString, strlen(errorString)); // \0 isn't sent.
}

// [1090;763.312s Recuperation Activated!]
void PmmErrorsAndSignals::reportRecuperation(unsigned long packageId)
{
    char recuperationString[ERROR_STRING_LENGTH];
    snprintf(recuperationString, ERROR_STRING_LENGTH, "%s[%lu;%.3fs %s]", RF_VALIDATION_HEADER_EXTRA, packageId, millis() / 1000.0, recuperationActivatedString);
    digitalWrite(PMM_PIN_LED_RECOVERY, HIGH);
    if (mSdIsWorking)
        mPmmSd->writeStringToFilename(mFilenameExtra, recuperationString + 4); // +4 to skip the MNEX header
    if (mTelemetryIsWorking)
        mPmmTelemetry->send((uint8_t*)recuperationString, strlen(recuperationString)); // \0 isn't sent.
}
void PmmErrorsAndSignals::generalMessage(const char* string, unsigned long packageId, int mSdIsWorking, int mTelemetryIsWorking)
{
    char customString[ERROR_STRING_LENGTH];
    snprintf(customString, ERROR_STRING_LENGTH, "%s[%lu;%.3fs %s]", RF_VALIDATION_HEADER_EXTRA, packageId, millis() / 1000.0, string);
    if (mSdIsWorking)
        mPmmSd->writeStringToFilename(mFilenameExtra, customString + 4); // +4 to skip the MNEX header
    if (mTelemetryIsWorking)
        mPmmTelemetry->send((uint8_t*)customString, strlen(customString)); // \0 isn't sent.
}

void PmmErrorsAndSignals::blinkRfLED(int state)
{
    if (!mActualNumberOfErrors)
        digitalWrite(PMM_PIN_ALL_OK_AND_TELEMETRY, state);
}

int PmmErrorsAndSignals::returnNumberOfErrors()
{
    return mActualNumberOfErrors;
}

// Getters
int getSdIsWorking()
{
    return mSdIsWorking;
}
int getTelemetryIsWorking()
{
    return mTelemetryIsWorking;
}
int getBarometerIsWorking()
    return mBarometerIsWorking;
int getAccelerometerIsWorking()
    return mAccelerometerIsWorking;
int getGyroscopeIsWorking()
    return mGyroscopeIsWorking;
int getMagnetometerIsWorking()
    return mMagnetometerIsWorking;
int getGpsIsWorking()
    return mGpsIsWorking;


// Setters
void setSdIsWorking(value)
{
     mSdIsWorking;
}
void setTelemetryIsWorking(int value);
{
    mTelemetryIsWorking = value;
}
void setBarometerIsWorking(int value);
{
    mBarometerIsWorking = value;
}
void setAccelerometerIsWorking(int value);
{
    mAccelerometerIsWorking = value;
}
void setGyroscopeIsWorking(int value);
{
    mGyroscopeIsWorking = value;
}
void setMagnetometerIsWorking(int value);
{
    mMagnetometerIsWorking = value;
}
void setGpsIsWorking(int value);
{
    mGpsIsWorking = value;
}
