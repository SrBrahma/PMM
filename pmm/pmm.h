/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include <Arduino.h>
#include <pmm.h>
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmPackageLog.h>
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

class Pmm
{
private:
    uint32_t mPackageTimeMs, mPackageLogId;

    PmmImu mPmmImu;

    PmmPackageLog mPmmPackageLog;

    PmmErrorsCentral mPmmErrorsCentral;         /* Errors */

    #if PMM_USE_TELEMETRY                       /* Telemetry */
    PmmTelemetry mPmmTelemetry;
    #endif

    #if PMM_USE_GPS                             /* GPS */
    PmmGps mPmmGps;
    #endif

    #if PMM_USE_SD                              /* SD */
    PmmSd mPmmSd;
    #endif

public:
    void init();
    void update();
};

#endif
