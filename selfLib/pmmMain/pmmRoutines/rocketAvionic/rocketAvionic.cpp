// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC


#include <generalUnitsOps.h>
#include <measuresAnalyzer.h>

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

#include "pmmRoutines/rocketAvionic/rocketAvionic.h"

RoutineRocketAvionic::RoutineRocketAvionic() {}

void RoutineRocketAvionic::init()
{
    int initStatus = 0;
    if ((initStatus += mAltitudeAnalyzer.init(millisToMicros(30), millisToMicros(100), secondsToMicros(0.5)) ))
    {
        advPrintf("Fatal error! Failed to alloc memory to mAltitudeAnalyzer!");
    }

    mAltAnalyzerIndexes.liftOff = mAltitudeAnalyzer.addCondition(90, MeasuresAnalyzer::CheckType::FirstDerivative,
                                   MeasuresAnalyzer::Relation::AreGreaterThan, 0.5, MeasuresAnalyzer::Time::Second);

    mAltAnalyzerIndexes.drogue  = mAltitudeAnalyzer.addCondition(90, MeasuresAnalyzer::CheckType::FirstDerivative,
                                   MeasuresAnalyzer::Relation::AreLesserThan, 0.5, MeasuresAnalyzer::Time::Second);

    mGpsIsFirstAltitude = mGpsIsFirstCoord = mGpsIsFirstDate = true;
    mSessionId          = mMainLoopCounter = 0;
    mMillis             = millis();

    pinMode(33, OUTPUT); pinMode(34, OUTPUT); pinMode(35, OUTPUT);


    initStatus += mPmmTelemetry.init();
    initStatus += mPmmSd.init(mSessionId);
    initStatus += mPmmGps.init();
    initStatus += mPmmImu.init();

    initStatus += mPmmModuleDataLog.init(&mPmmTelemetry, &mPmmSd, mSessionId, 0, &mMainLoopCounter, &mMillis);
        mPmmModuleDataLog.getDataLogGroupCore()->addGps(mPmmGps.getGpsStructPtr());
        mPmmModuleDataLog.getDataLogGroupCore()->addImu(mPmmImu.getImuStructPtr());

    initStatus += mPmmModuleMessageLog.init(&mMainLoopCounter, &mPmmTelemetry, &mPmmSd); // PmmModuleMessageLog
    initStatus += mPmmPortsReception.init(&mPmmModuleDataLog, &mPmmModuleMessageLog);    // PmmPortsReception

    digitalWrite(33, !initStatus);
    
    recovery0DisableAtMillis = recovery1DisableAtMillis = 0;

    mMillis = millis(); // Again!

    printMotd();
}

void RoutineRocketAvionic::update()
{
    if (recovery0DisableAtMillis && (millis() > recovery0DisableAtMillis))
    { recovery0DisableAtMillis = 0; // disableRec0
    }
    if (recovery1DisableAtMillis && (millis() > recovery1DisableAtMillis))
    { recovery1DisableAtMillis = 0; // disableRec0
    }

    switch(mSubRoutine)
    {
        case SubRoutines::FullActive:
            sR_FullActive();   break;
        case SubRoutines::Landed:
            sR_Landed();        break;
    }

    //PMM_DEBUG_PRINTF("Time passed = %lums. Id is %lu\n", millis() - mMillis, mMainLoopCounter);

    mMainLoopCounter++; mMillis = millis();

}

void RoutineRocketAvionic::sR_FullActive()
{
    int imuRtn = mPmmImu.update();

    if (imuRtn & PmmImu::BarGotPressure) {
        mAltitudeAnalyzer.addMeasure(mPmmImu.getAltitudeBarometer());
        PMM_DEBUG_PRINTF("added another bar measure after %lu ms\n", millis() - lastAddedBarAtMillis);
        lastAddedBarAtMillis = millis();

        if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.liftOff))
            digitalWrite(34, 1);
        if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.drogue))
            digitalWrite(35, 1);
            Serial.println();
    }



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


// "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
void RoutineRocketAvionic::printMotd()
{
    #if PMM_DEBUG
        if (!Serial)
            return;

        PMM_DEBUG_PRINTLN("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=-\n\n");

        #if PMM_DATA_LOG_DEBUG
            mPmmModuleDataLog.debugPrintLogHeader();
            PMM_DEBUG_PRINTLN();
        #endif

        #if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED
            PMM_DEBUG_PRINTF("Pmm: Press any key to continue the code. (set PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED (pmmConsts.h) to 0 to disable this!)\n\n");
            for (; !Serial.available(); delay(10));

        #elif PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT
            PMM_DEBUG_PRINTF("Pmm: System is halted for %i ms so you can read the init messages.\n\n", PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT)
            delay(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
        #endif

        PMM_DEBUG_PRINTLN("Main loop started!");

    #endif
}


#endif