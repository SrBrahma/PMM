/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmErrorsCentral.h>
#include <pmmHealthSignals.h>



#if PMM_USE_TELEMETRY
    #include <pmmTelemetry.h>
#endif

#if PMM_USE_GPS
    #include <pmmGps.h>
#endif

#if PMM_USE_SD
    #include <pmmSd.h>
#endif

//Packages
#include "pmmTelemetryPorts/pmmPortLog.h"
#include "pmmTelemetryPorts/pmmPortString.h"
#include "pmmTelemetryPorts/pmmPortsReception.h"



class Pmm
{

public:
    void init();
    void update();

private:
    uint32_t mPackageTimeMs, mPackageLogId;

    PmmImu mPmmImu;

    // Packages
    PmmPortLog mPmmPortLog;
    PmmPortString mPmmPortString;
    PmmPortsReception mPmmPortsReception;

    PmmErrorsCentral mPmmErrorsCentral;         /* Errors */

    #if PMM_USE_TELEMETRY                       /* Telemetry */
    PmmTelemetry mPmmTelemetry;
    #endif

    #if PMM_USE_GPS                             /* GPS */
    PmmGps mPmmGps;
    #endif

    //#if PMM_USE_SD       // is commented so it can be an argument for the packages objects init                       /* SD */
    PmmSd mPmmSd;
    //#endif

};

#endif
