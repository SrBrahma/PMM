// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#ifndef PMM_ROUTINE_PDA_h
#define PMM_ROUTINE_PDA_h

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_PDA


#include "pmmTelemetry/telemetry.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"
#include "pmmSd/sd.h"

// Modules
#include "pmmModules/simpleDataLog/receiver.h"

#include "LiquidCrystal_I2C.h" // For the 2004 char display

class RoutinePda
{
public:

    RoutinePda();

    void init();
    void update();

private:
    // Add the values to the Module SimpleDataLog
    void addVarsSimpleDataLog();
    
    // If the reception was without errors, returns the received PORT_ID (ex PORT_ID_SIMPLE_DATA_LOG).
    // If wasn't a valid packet or any other issue, returns 0.
    int  updateTelemetryReception();

    // Updates every variables used in the display. Tx GPS pos, bearing to it, etc.
    void updatePdaData();

    // Writes everything to the display.
    void updateDisplay();

    // "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
    void printMotd();


    LiquidCrystal_I2C mLiquidCrystal;

    uint8_t      mSessionId;
    uint32_t     mMainLoopCounter, mMillis;


    // Main objects
    PmmTelemetry mPmmTlm;
    PmmImu       mPmmImu;
    PmmGps       mPmmGps;
    PmmSd        mPmmSd;

    // Modules
    ModuleSimpleDataLogRx mSimpleDataLogRx;

    uint32_t     mLastMillisRefreshedDisplay;

    struct {
        uint32_t txCounter;
        uint32_t mainLoopCounter;
        uint32_t timeMillis;
        int32_t  gpsLat;
        int32_t  gpsLon;
        uint32_t lastGpsLocationTimeMs;
        float    barometerAltitude;
    } mTxData;

    struct {
        int      txCounter;
        int      mainLoopCounter;
        int      timeMillis;
        int      gpsLat;
        int      gpsLon;
        int      lastGpsLocationTimeMs;
        int      barometerAltitude;
    } mTxIndexes;

};

#endif
#endif
