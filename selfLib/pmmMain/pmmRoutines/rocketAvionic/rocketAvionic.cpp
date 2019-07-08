// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC

#include "pmmHealthSignals/healthSignals.h"

#include "pmmEeprom/eeprom.h"

#include "pmmTelemetry/telemetry.h"
#include "pmmModules/portsReception.h"
#include "pmmSd/sd.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"

// Modules
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"

#include "pmmDebug.h"   // For debug prints

#include "pmmRoutines/rocketAvionic/recovery/recovery.h"
#include "pmmRoutines/rocketAvionic/rocketAvionic.h"

RoutineRocketAvionic::RoutineRocketAvionic() {}

void RoutineRocketAvionic::init()
{
    mGpsIsFirstAltitude = mGpsIsFirstCoord = mGpsIsFirstDate = true;
    mSessionId          = mMainLoopCounter = 0;
    mMillis             = millis();

    mPmmTelemetry.init();
    mPmmSd.init(mSessionId);
    mPmmGps.init();
    mPmmImu.init();

    mPmmModuleDataLog.init(&mPmmTelemetry, &mPmmSd, mSessionId, 0, &mMainLoopCounter, &mMillis);
        mPmmModuleDataLog.getDataLogGroupCore()->addGps(mPmmGps.getGpsStructPtr());
        mPmmModuleDataLog.getDataLogGroupCore()->addImu(mPmmImu.getImuStructPtr());

    mPmmModuleMessageLog.init(&mMainLoopCounter, &mPmmTelemetry, &mPmmSd); // PmmModuleMessageLog
    mPmmPortsReception.init(&mPmmModuleDataLog, &mPmmModuleMessageLog);    // PmmPortsReception

    recovery0MillisRemaining = recovery1MillisRemaining = 0;

    mMillis = millis(); // Again!
}

void RoutineRocketAvionic::update()
{
    if (recovery0MillisRemaining)
        recovery0MillisRemaining -= (recovery0MillisRemaining < (millis() - mMillis)? ;
    uint32_t recovery1MillisRemaining;
    switch(mSubRoutine)
    {
        case SubRoutines::AwaitingGps:
            sR_AwaitingGps();   break;
        case SubRoutines::Landed:
            sR_Landed();        break;
    }
    mMainLoopCounter++;
    mMillis = millis();

    PMM_DEBUG_PRINTF("\n\n");
    delay(500);
}

void RoutineRocketAvionic::sR_AwaitingGps()
{
    mPmmImu.update();
    if (mPmmGps.update() == PmmGps::UpdateRtn::GotFix)
    {
        if (mGpsIsFirstCoord && mPmmGps.getFixPtr()->valid.location) {
            mPmmImu.setDeclination(mPmmGps.getGpsStructPtr()->latitude, mPmmGps.getGpsStructPtr()->longitude);
            mGpsIsFirstCoord = false; }

        // if (mGpsIsFirstAltitude && mPmmGps.getFixPtr()->valid.altitude) {
        //     // calibrate barometer to get real altitude
        //     mGpsIsFirstAltitude = false; }

        // if (mGpsIsFirstDate && mPmmGps.getFixPtr()->valid.date) {
        //     // save the date as message.
        //     mGpsIsFirstDate = false; }
    }
    mPmmModuleDataLog.update();
    mPmmModuleDataLog.debugPrintLogContent(); // There is on the start #if PMM_DEBUG && PMM_DATA_LOG_DEBUG

    if(mPmmTelemetry.updateReception())
        mPmmPortsReception.receivedPacket(mPmmTelemetry.getReceivedPacketAllInfoStructPtr());
    mPmmTelemetry.updateTransmission();
}



void RoutineRocketAvionic::sR_Landed()
{
}






#endif