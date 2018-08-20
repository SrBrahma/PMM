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
#include "pmmPackages/pmmPackageLog.h"
#include "pmmPackages/pmmPackageString.h"
#include "pmmPackages/pmmPackagesReception.h"



class Pmm
{
private:
    uint32_t mPackageTimeMs, mPackageLogId;

    PmmImu mPmmImu;

    // Packages
    PmmPackageLog mPmmPackageLog;
    PmmPackageString mPmmPackageString;
    PmmPackagesReception mPmmPackagesReception;

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

public:
    void init();
    void update();
};

#endif
