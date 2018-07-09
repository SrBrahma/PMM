/* As the code is quite small, I made it just in this .h file.
08/07/2018 Henrique Bruno */

#ifndef PMM_ERRORS_CENTRAL_h
#define PMM_ERRORS_CENTRAL_h

#include <pmmConsts.h>


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

const PROGMEM char *pmmErrorString[ERRORS_AMOUNT] = {
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
const PROGMEM char recuperationActivatedString[] = "Recuperation Activated!";



class PmmErrorsCentral
{
private:
    #define PMM_ERRORS_ARRAY_INDEX_PACKAGE_LOG_ID 0
    #define PMM_ERRORS_ARRAY_INDEX_MILLIS 1
    #define PMM_ERRORS_ARRAY_INDEX_ERROR_ID 2

    pmmErrorType mErrorsArray[ERRORS_ARRAY_SIZE][3]; // Total erros in the system.
    // [x][0] was the current Package Log Id
    // [x][1] was the current millis
    // [x][2] is the error identifier

    int mActualNumberOfErrors; // Total errors in the system number
    int mSdIsWorking,  mGpsIsWorking, mTelemetryIsWorking, mBarometerIsWorking, mAccelerometerIsWorking, mGyroscopeIsWorking, mMagnetometerIsWorking; // are int for faster access.

    const uint32_t* mPackageLogIdPtr;

    void addError()
    {
        mErrorsArray[mActualNumberOfErrors][0] = &mPackageLogIdPtr;
    }

public:
    PmmErrorsCentral()
    {
    }

    int init (const uint32_t* packageLogIdPtr)
    {
        mPackageLogIdPtr = packageLogIdPtr;
        mActualNumberOfErrors = 0;
        mSdIsWorking = mGpsIsWorking = mTelemetryIsWorking = mBarometerIsWorking = mAccelerometerIsWorking = mGyroscopeIsWorking = mMagnetometerIsWorking = 1; // All systems starts functional

    }

    // Getters
    int getNumberOfErrors() { return mActualNumberOfErrors; }
    int getSdIsWorking() { return mSdIsWorking; }
    int getTelemetryIsWorking() { return mTelemetryIsWorking; }
    int getBarometerIsWorking() { return mBarometerIsWorking; }
    int getAccelerometerIsWorking() { return mAccelerometerIsWorking; }
    int getGyroscopeIsWorking() { return mGyroscopeIsWorking; }
    int getMagnetometerIsWorking() { return mMagnetometerIsWorking; }
    int getGpsIsWorking() { return mGpsIsWorking; }

    // Setters
    void setSdIsWorking(int value) { mSdIsWorking = value; }
    void setTelemetryIsWorking(int value) { mTelemetryIsWorking = value; }
    void setBarometerIsWorking(int value) { mBarometerIsWorking = value; }
    void setAccelerometerIsWorking(int value) { mAccelerometerIsWorking = value; }
    void setGyroscopeIsWorking(int value) { mGyroscopeIsWorking = value; }
    void setMagnetometerIsWorking(int value) { mMagnetometerIsWorking = value; }
    void setGpsIsWorking(int value) { mGpsIsWorking = value; }
};

#endif
