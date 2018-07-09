/* pmmErrorsCentral.h
 * As the code is quite small, I wrote it entirely just in this .h file.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_ERRORS_CENTRAL_h
#define PMM_ERRORS_CENTRAL_h

#include <pmmConsts.h>


#define ERRORS_ARRAY_SIZE 20
#define ERROR_STRING_LENGTH 80

class PmmErrorsCentral
{
private:
    pmmErrorStructType mErrorsArray[ERRORS_ARRAY_SIZE]; // Total erros in the system.


    int mActualNumberOfErrors; // Total errors in the system number
    int mSdIsWorking,  mGpsIsWorking, mTelemetryIsWorking, mBarometerIsWorking, mAccelerometerIsWorking, mGyroscopeIsWorking, mMagnetometerIsWorking; // are int for faster access.

    const uint32_t* mPackageLogIdPtr;

    void addError(pmmErrorCodeType errorCode)
    {
        if (errorCode < 0)
            errorCode = ERROR_PROGRAMMING;
        if (errorCode > ERRORS_CODE_AMOUNT)
            errorCode = ERROR_PROGRAMMING;

        mErrorsArray[mActualNumberOfErrors].code = errorCode;
        mErrorsArray[mActualNumberOfErrors].timeMs = millis();
        mErrorsArray[mActualNumberOfErrors].packageLogId = &mPackageLogIdPtr;
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
        return 0;
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
