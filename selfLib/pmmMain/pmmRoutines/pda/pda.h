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



class RoutinePda
{
public:

    RoutinePda();

    void init();
    void update();

private:
    // Add the values to the Module SimpleDataLog
    void addVarsSimpleDataLog();

    // "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
    void printMotd();


    uint8_t      mSessionId;
    uint32_t     mMainLoopCounter, mMillis;


    // Main objects
    PmmTelemetry mPmmTelemetry;
    PmmImu       mPmmImu;
    PmmGps       mPmmGps;
    PmmSd        mPmmSd;

    // Modules
    PortsReception        mPortsReception;
    ModuleSimpleDataLogRx mSimpleDataLogRx;

    struct mReceivedVars {
        int      transmissionCounterIndex;
        uint32_t gpsNoCoordMillis;
        int      gpsNoCoordMillisIndex;
        int      gpsLatIndex;
        int      gpsLonIndex;
        int      gpsSatsIndex;
        int      barAltIndex;
    };

};

#endif
#endif
