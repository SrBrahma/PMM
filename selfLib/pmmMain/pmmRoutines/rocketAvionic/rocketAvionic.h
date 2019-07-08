/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_ROUTINE_ROCKET_AVIONIC_h
#define PMM_ROUTINE_ROCKET_AVIONIC_h

#include <pmmConsts.h>

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC




#include "pmmHealthSignals/healthSignals.h"


#include "pmmTelemetry/telemetry.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"
#include "pmmSd/sd.h"

// Modules
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"
#include "pmmModules/portsReception.h"



class RoutineRocketAvionic
{

public:

    RoutineRocketAvionic();

    void init();
    void update();


private:

    enum class SubRoutines {FullActive, Landed};
    SubRoutines  mSubRoutine;
    void setSubRoutine(SubRoutines subRoutine);
    void sR_FullActive();
    void sR_Landed();


    uint8_t      mSessionId;
    uint32_t     mMainLoopCounter, mMillis;

    // Main objects
    PmmTelemetry mPmmTelemetry;
    PmmImu       mPmmImu;
    PmmGps       mPmmGps;
    PmmSd        mPmmSd;

    // Modules
    PmmModuleMessageLog mPmmModuleMessageLog;
    PmmPortsReception   mPmmPortsReception;
    PmmModuleDataLog    mPmmModuleDataLog;

    bool mGpsIsFirstAltitude;
    bool mGpsIsFirstCoord;
    bool mGpsIsFirstDate;

    uint32_t recovery0MillisRemaining;
    uint32_t recovery1MillisRemaining;
};

#endif
#endif
