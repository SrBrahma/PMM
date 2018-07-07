#ifndef PMM_ERRORS_AND_SIGNALS_h
#define PMM_ERRORS_AND_SIGNALS_h

#include <RH_RF95.h>
#include <pmmConsts.h>
#include <pmmSd.h>

//--------------Error variables---------------//
#define ERRORS_ARRAY_SIZE 20
#define ERROR_STRING_LENGTH 80

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

class PmmErrorsAndSignals
{
private:
    PmmSd *mPmmSd;
    PmmTelemetry *mPmmTelemetry; // Pointer to the RF object

    pmmErrorType mErrorsArray[ERRORS_ARRAY_SIZE]; // Total erros in the system
    int mActualNumberOfErrors; // Total errors in the system number
    char mFilenameExtra[PMM_SD_FILENAME_MAX_LENGTH]; // Filename of this extra log

    int mSystemWasOk, mSignalIsOn, mSignalStarterCounter, mSignalActualErrorIndex, mSignalActualErrorCounter, mIsShortBeepOfSystemWasOk; // Used updateLedsAndBuzzer
    unsigned long mMillisNextSignalState; // Used updateLedsAndBuzzer

    int mSdIsWorking,  mGpsIsWorking, mTelemetryIsWorking, mBarometerIsWorking, mAccelerometerIsWorking, mGyroscopeIsWorking, mMagnetometerIsWorking; // are int for faster access.


public:
    PmmErrorsAndSignals();
    void init(SdManager *sdManager, RH_RF95 *rf95Ptr, uint16_t fileID);
    void updateLedsAndBuzzer();
    void reportError(pmmErrorType errorID, unsigned long packageId);
    void reportRecuperation(unsigned long packageId);
    void generalMessage(const char *string, unsigned long packageId);
    void blinkRfLED(int state);
    int returnNumberOfErrors();

    // Getters
    int getSdIsWorking();
    int getTelemetryIsWorking();
    int getBarometerIsWorking();
    int getAccelerometerIsWorking();
    int getGyroscopeIsWorking();
    int getMagnetometerIsWorking();
    int getGpsIsWorking();


    // Setters
    void setSdIsWorking(int value);
    void setTelemetryIsWorking(int value);
    void setBarometerIsWorking(int value);
    void setAccelerometerIsWorking(int value);
    void setGyroscopeIsWorking(int value);
    void setMagnetometerIsWorking(int value);
    void setGpsIsWorking(int value);
};

#endif
