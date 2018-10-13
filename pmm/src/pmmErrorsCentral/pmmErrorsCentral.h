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


const PROGMEM char recuperationActivatedString[] = "Recuperation Activated!";

class PmmErrorsCentral
{
private:
    pmmErrorStructType mErrorsStructArray[ERRORS_ARRAY_SIZE]; // Total erros in the system.

    int mActualNumberOfErrors; // Total errors in the system number
    int mSdIsWorking, mTelemetryIsWorking, mGpsIsWorking, mBarometerIsWorking, mAccelerometerIsWorking, mGyroscopeIsWorking, mMagnetometerIsWorking; // are int for faster access.

    const uint32_t* mPackageLogIdPtr;

    void addError(pmmErrorCodeType errorCode);

public:
    PmmErrorsCentral();

    int init (const uint32_t* packageLogIdPtr);

    // Getters
    const pmmErrorStructType* getErrorsStructArray();
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
