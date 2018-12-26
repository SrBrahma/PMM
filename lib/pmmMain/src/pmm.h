/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include <pmmConsts.h>

#include "pmmHealthBasicSignals/pmmHealthBasicSignals.h"

#if PMM_USE_TELEMETRY
    #include "pmmTelemetry/pmmTelemetry.h"
#endif

#if PMM_USE_IMU
    #include "pmmImu/pmmImu.h"
#endif

#if PMM_USE_GPS
    #include "pmmGps/pmmGps.h"
#endif

#if PMM_USE_SD
    #include "pmmSd/pmmSd.h"
#endif

//Packages
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"
#include "pmmModules/portsReception.h"



class Pmm
{

public:

    Pmm();

    int  init(bool skipDebugDelay = false);
    void update();

private:

    int setSystemMode(pmmSystemState systemMode);

    uint8_t  mSessionId;
    uint32_t mMillis, mMainLoopCounter;

    pmmSystemState mSystemState;

    #if PMM_USE_IMU
        PmmImu mPmmImu;
    #endif

    #if PMM_USE_TELEMETRY
        PmmTelemetry mPmmTelemetry;
    #endif

    #if PMM_USE_GPS
        PmmGps mPmmGps;
    #endif

    #if PMM_USE_SD
        PmmSd mPmmSd;
    #endif

        // Packages
    PmmModuleDataLog    mPmmModuleDataLog;
    PmmModuleMessageLog mPmmModuleMessageLog;
    PmmPortsReception   mPmmPortsReception;
    
};

#endif
