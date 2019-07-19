// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#ifndef PMM_ROUTINE_ROCKET_AVIONIC_h
#define PMM_ROUTINE_ROCKET_AVIONIC_h

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC

#include <measuresAnalyzer.h>

#include "pmmTelemetry/telemetry.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"
#include "pmmSd/sd.h"

// Modules
#include "pmmModules/simpleDataLog/transmitter.h"
#include "pmmModules/ports.h"



class RoutineRocketAvionic
{
public:

    RoutineRocketAvionic();

    void init();
    void update();

private:
    // Add the values to the Module SimpleDataLog
    void addVarsSimpleDataLog();

    // "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
    void printMotd();

    enum class SubRoutines {FullActive, Landed};
    SubRoutines  mSubRoutine;

    void setSubRoutine(SubRoutines subRoutine);
    void sR_FullActive();
    void sR_Landed();

    void deployRecoveriesIfConditionsMet(uint32_t timeMillis, float altitude);
    void disableRecDeployIfTimePassed(uint32_t timeMillis);

    uint8_t      mSessionId;
    uint32_t     mMainLoopCounter, mMillis;

    MeasuresAnalyzer mAltitudeAnalyzer;
    struct { int liftOff, drogue, mainAlt, mainVel;  } mAltAnalyzerIndexes;
    struct { bool liftOff, drogue, main; } mDetections;
    struct { bool drogue, main;          } mDeploying;
    struct { uint32_t drogue, main;      } mRecoveryStopDeployAtMillis;


    // Main objects
    PmmTelemetry mPmmTelemetry;
    PmmImu       mPmmImu;
    PmmGps       mPmmGps;
    PmmSd        mPmmSd;

    // Modules
    PortsReception mPortsReception;
    ModuleSimpleDataLogTx mSimpleDataLogTx;

};

#endif
#endif
