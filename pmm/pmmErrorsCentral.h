/* pmmErrorsCentral.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_ERRORS_CENTRAL_h
#define PMM_ERRORS_CENTRAL_h

#include <pmmConsts.h>


#define ERRORS_ARRAY_SIZE 20
#define ERROR_STRING_LENGTH 80


typedef enum
{
    OK,
    ERROR_SD,
    ERROR_SD_WRITE,
    ERROR_RF_INIT,
    ERROR_RF_SET_FREQ,
    ERROR_GPS,
    ERROR_ACCELEROMETER_INIT,
    ERROR_GYROSCOPE_INIT,
    ERROR_MAGNETOMETER_INIT,
    ERROR_BAROMETER_INIT,
    ERROR_PROGRAMMING,
    ERRORS_CODE_AMOUNT
} pmmErrorCodeType;

typedef struct
{
    uint32_t timeMs;          // When (millis()) it ocurred.
    uint32_t packageLogId;  // At which package log the error ocurred
    pmmErrorCodeType code;  // The error code (pmmErrorCodeType)
} pmmErrorStructType; // This struct is a little bad: some bytes are being used as padding.

const PROGMEM char *pmmErrorString[ERRORS_CODE_AMOUNT] = {
    "No errors",                // OK,
    "SD init fail",             // ERROR_SD,
    "SD write fail",            // ERROR_SD_WRITE,
    "RF init fail",             // ERROR_RF_INIT,
    "RF Set Freq fail",         // ERROR_RF_SET_FREQ,
    "GPS fail",                 // ERROR_GPS
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
    pmmErrorStructType mErrorsArray[ERRORS_ARRAY_SIZE]; // Total erros in the system.

    int mActualNumberOfErrors; // Total errors in the system number
    int mSdIsWorking, mTelemetryIsWorking, mGpsIsWorking, mBarometerIsWorking, mAccelerometerIsWorking, mGyroscopeIsWorking, mMagnetometerIsWorking; // are int for faster access.

    const uint32_t* mPackageLogIdPtr;

    void addError(pmmErrorCodeType errorCode);

public:
    PmmErrorsCentral();

    int init (const uint32_t* packageLogIdPtr);

    // Getters
    pmmErrorStructType getErrorStruct(int errorIndex);  // Returns the struct of the error of the given index.
    void getErrorStructPtr(); // Returns the pointer of the struct
    int getNumberOfErrors();
    int getSdIsWorking();
    int getTelemetryIsWorking();
    int getBarometerIsWorking();
    int getAccelerometerIsWorking();
    int getGyroscopeIsWorking();
    int getMagnetometerIsWorking();
    int getGpsIsWorking();

    // Setters
    void reportErrorByCode(pmmErrorCodeType errorCode);
    void setSdIsWorking(int value);
    void setTelemetryIsWorking(int value);
    void setBarometerIsWorking(int value);
    void setAccelerometerIsWorking(int value);
    void setGyroscopeIsWorking(int value);
    void setMagnetometerIsWorking(int value);
    void setGpsIsWorking(int value);
};

#endif
