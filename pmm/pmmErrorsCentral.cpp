/* pmmErrorsCentral.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmErrorsCentral.h>

const PROGMEM char *pmmErrorString[ERRORS_CODE_AMOUNT] = {
    "No errors",                // OK,
    "SD init fail",             // ERROR_SD,
    "SD write fail",            // ERROR_SD_WRITE,
    "RF init fail",             // ERROR_RF_INIT,
    "GPS fail",                 // ERROR_GPS
    "Accelerometer init fail",  // ERROR_ACCELEROMETER_INIT,
    "Gyroscope init fail",      // ERROR_GYROSCOPE_INIT,
    "Magnetometer init fail",   // ERROR_MAGNETOMETER_INIT,
    "Barometer init fail",      // ERROR_BAROMETER_INIT
    "Programming error"         // ERROR_PROGRAMMING
};

void PmmErrorsCentral::addError(pmmErrorCodeType errorCode)
{
    // No need to treat the values, as them will be treated on the reportErrorByCodeByCode().
    mErrorsStructArray[mActualNumberOfErrors].code = errorCode;
    mErrorsStructArray[mActualNumberOfErrors].timeMs = millis();
    mErrorsStructArray[mActualNumberOfErrors].packageLogId = *mPackageLogIdPtr;
    mActualNumberOfErrors++;
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
const pmmErrorStructType* PmmErrorsCentral::getErrorsStructArray() { return mErrorsStructArray; }
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
