/* pmmErrorsCentral.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmErrorsCentral.h>

void PmmErrorsCentral::addError(pmmErrorCodeType *errorCode) // It passes the pointer as I have a plan
// on the future to get the previous state of the error, and sending a String package of this error being new.
{
    // No need to treat the values, as them will be treated on the reportErrorByCode().

    mErrorsArray[mActualNumberOfErrors].code = errorCode;
    mErrorsArray[mActualNumberOfErrors].timeMs = millis();
    mErrorsArray[mActualNumberOfErrors].packageLogId = &mPackageLogIdPtr;
}

PmmErrorsCentral::PmmErrorsCentral()
{
}

int PmmErrorsCentral::init (const uint32_t* packageLogIdPtr)
{
    mPackageLogIdPtr = packageLogIdPtr;
    mActualNumberOfErrors = 0;
    mSdIsWorking = mTelemetryIsWorking = mGpsIsWorking = mBarometerIsWorking = mAccelerometerIsWorking = mGyroscopeIsWorking = mMagnetometerIsWorking = 1; // All systems starts functional
    return 0;
}

// Getters
int PmmErrorsCentral::getNumberOfErrors() { return mActualNumberOfErrors; }
int PmmErrorsCentral::getSdIsWorking() { return mSdIsWorking; }
int PmmErrorsCentral::getTelemetryIsWorking() { return mTelemetryIsWorking; }
int PmmErrorsCentral::getGpsIsWorking() { return mGpsIsWorking; }
int PmmErrorsCentral::getBarometerIsWorking() { return mBarometerIsWorking; }
int PmmErrorsCentral::getAccelerometerIsWorking() { return mAccelerometerIsWorking; }
int PmmErrorsCentral::getGyroscopeIsWorking() { return mGyroscopeIsWorking; }
int PmmErrorsCentral::getMagnetometerIsWorking() { return mMagnetometerIsWorking; }

// Setters
void PmmErrorsCentral::reportErrorByCode(pmmErrorCodeType errorCode)
{
    switch(errorCode)
    {
        case (OK):
            break;

        case (ERROR_SD):
            mSdIsWorking = 0;
            break;

        case (ERROR_SD_WRITE):
            mSdIsWorking = 0;
            break;

        case (ERROR_GPS):
            mGpsIsWorking = 0;
            break;

        case (ERROR_RF_INIT):
            mTelemetryIsWorking = 0;
            break;

        case (ERROR_RF_SET_FREQ):
            mTelemetryIsWorking = 0;
            break;

        case (ERROR_ACCELEROMETER_INIT):
            mAccelerometerIsWorking = 0;
            break;

        case (ERROR_GYROSCOPE_INIT):
            mGyroscopeIsWorking = 0;
            break;

        case (ERROR_MAGNETOMETER_INIT):
            mMagnetometerIsWorking = 0;
            break;

        case (ERROR_BAROMETER_INIT):
            mBarometerIsWorking = 0;
            break;

        default:
            break;
    } // End of switch
} // End of function

void PmmErrorsCentral::setSdIsWorking(int value)
{
    mSdIsWorking = value;
}

void PmmErrorsCentral::setTelemetryIsWorking(int value)
{
    mTelemetryIsWorking = value;
}

void PmmErrorsCentral::setBarometerIsWorking(int value)
{
    mBarometerIsWorking = value;
}

void PmmErrorsCentral::setAccelerometerIsWorking(int value)
{
    mAccelerometerIsWorking = value;
}

void PmmErrorsCentral::setGyroscopeIsWorking(int value)
{
    mGyroscopeIsWorking = value;
}

void PmmErrorsCentral::setMagnetometerIsWorking(int value)
{
    mMagnetometerIsWorking = value;
}

void PmmErrorsCentral::setGpsIsWorking(int value)
{
    mGpsIsWorking = value;
}
