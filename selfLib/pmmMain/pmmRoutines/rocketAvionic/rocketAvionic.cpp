// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC

#include <EEPROM.h> // To get the mSessionId
#include <generalUnitsOps.h>
#include <measuresAnalyzer.h>

#include "pmmTelemetry/telemetry.h"
#include "pmmSd/sd.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"

// Modules
#include "pmmModules/ports.h"
#include "pmmModules/simpleDataLog/transmitter.h"

#include "pmmDebug.h"   // For debug prints

#include "pmmRoutines/rocketAvionic/rocketAvionicConsts.h"
#include "pmmRoutines/rocketAvionic/rocketAvionic.h"


RoutineRocketAvionic::RoutineRocketAvionic() {}


void RoutineRocketAvionic::init()
{
    mMainLoopCounter = 0;
    mMillis = millis();

    int initStatus = 0;

    int adrs = 0;
    mSessionId = EEPROM.read(adrs); 
    EEPROM.write(adrs, mSessionId + 1);
    EEPROM.read(adrs);

    // 2) Main objects
    initStatus += mPmmTlm.init();
    initStatus += mPmmSd.init(mSessionId);
    initStatus += mPmmGps.init();
    initStatus += mPmmImu.init();

    // 3) Modules
    mSimpleDataLogTx.init(&mPmmTlm, &mPmmSd, mSessionId);
    addVarsSimpleDataLog();


    // 4) Recovery. 20ms as the minTime is a nice value. BMP085/180 has a min value of ~26ms between
    // each measure on the Ultra-etc precision mode -- the one used here.
    if (mAltitudeAnalyzer.init(millisToMicros(20), millisToMicros(100), secondsToMicros(1), 10)) {
        initStatus ++;
        advPrintf("Fatal error! Failed to alloc memory to mAltitudeAnalyzer!");
    }
    mAltAnalyzerIndexes.liftOff = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreGreaterThan, 5, MeasuresAnalyzer::Time::Second);
    mAltAnalyzerIndexes.drogue  = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreLesserThan, -5, MeasuresAnalyzer::Time::Second);
    mAltAnalyzerIndexes.mainAlt = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::Values,
                                    MeasuresAnalyzer::Relation::AreLesserThan, 650);
    mAltAnalyzerIndexes.mainVel = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreLesserThan, -5, MeasuresAnalyzer::Time::Second);
    mAltAnalyzerIndexes.mainVel2 = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreLesserThan, -150, MeasuresAnalyzer::Time::Second);


    pinMode(ROCKET_AVIONIC_PIN_DROGUE, OUTPUT); pinMode(ROCKET_AVIONIC_PIN_MAIN, OUTPUT);
    digitalWrite(ROCKET_AVIONIC_PIN_DROGUE, 0); digitalWrite(ROCKET_AVIONIC_PIN_MAIN, 0); // Just to ensure! 

    mDetections.liftOff = mDetections.drogue = mDetections.main = false;
    mDeploying.drogue   = mDeploying.main    = false;
    mRecoveryStopDeployAtMillis.drogue = mRecoveryStopDeployAtMillis.main = 0;

    // 5) End!
    mMillis = millis(); // Again!
    printMotd();
}

void RoutineRocketAvionic::update()
{
    switch(mSubRoutine)
    {
        case SubRoutines::FullActive:
            sR_FullActive();   break;
        case SubRoutines::Landed:
            sR_Landed();        break;
    }

    mMainLoopCounter++; mMillis = millis();
}

void RoutineRocketAvionic::sR_FullActive()
{
    disableRecDeployIfTimePassed(mMillis);
    // Check if we have a new pressure measure
    if (mPmmImu.update() & PmmImu::BarGotPressure)
        deployRecoveriesIfConditionsMet(mMillis, mPmmImu.getBarometerAltitude());
    mPmmGps.update(mMillis);
    mSimpleDataLogTx.send();
    mSimpleDataLogTx.storeOnSd();
}


// Don't want to do it right now. But, should a be subroutine where it uses far less energy, to keep the system
// alive for a looooong time.
void RoutineRocketAvionic::sR_Landed()
{
}


// This MUST be exactly the same, for both transmitter and receiver.
void RoutineRocketAvionic::addVarsSimpleDataLog()
{
    mSimpleDataLogTx.addBasicInfo(&mMainLoopCounter, &mMillis);
    mSimpleDataLogTx.addAccelerometer(mPmmImu.getAccelerometerPtr());
    mSimpleDataLogTx.addGyroscope(mPmmImu.getGyroscopePtr());
    mSimpleDataLogTx.addMpuTemperature(mPmmImu.getMpuTemperaturePtr());
    mSimpleDataLogTx.addBarometerPressure(mPmmImu.getBarometerPressurePtr());
    mSimpleDataLogTx.addBarometerAltitude(mPmmImu.getBarometerAltitudePtr());
    mSimpleDataLogTx.addMagnetometer(mPmmImu.getMagnetometerPtr());
    mSimpleDataLogTx.addGpsLatLong(&mPmmGps.getGpsStructPtr()->latitude, &mPmmGps.getGpsStructPtr()->longitude);
    mSimpleDataLogTx.addGpsLastLocationTimeMs(mPmmGps.getLastLocationTimeMsPtr());
    mSimpleDataLogTx.addGpsAltitude(&mPmmGps.getGpsStructPtr()->altitude);
    mSimpleDataLogTx.addGpsSatellites(&mPmmGps.getGpsStructPtr()->satellites);
}


void RoutineRocketAvionic::disableRecDeployIfTimePassed(uint32_t timeMs)
{
    if (mDeploying.drogue && (timeMs > mRecoveryStopDeployAtMillis.drogue)) {
        mRecoveryStopDeployAtMillis.drogue = 0;
        mDeploying.drogue = false;
        digitalWrite(ROCKET_AVIONIC_PIN_DROGUE, 0);
    }
    if (mDeploying.main   && (timeMs > mRecoveryStopDeployAtMillis.main  )) {
        mRecoveryStopDeployAtMillis.main   = 0;
        mDeploying.main   = false;
        digitalWrite(ROCKET_AVIONIC_PIN_MAIN,   0);
    }
}

void RoutineRocketAvionic::deployRecoveriesIfConditionsMet(uint32_t timeMs, float altitude)
{
    mAltitudeAnalyzer.addMeasure(altitude, millisToMicros(timeMs));

    if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.liftOff)) {
        mDetections.liftOff = true;
    }
    if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.drogue )) {
        mDetections.drogue = true;
        mDeploying.drogue  = true;
        mRecoveryStopDeployAtMillis.drogue = timeMs + ROCKET_AVIONIC_DROGUE_ACTIVE_TIME_MS;
        digitalWrite(ROCKET_AVIONIC_PIN_DROGUE, 1);
    }
    if ((mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.mainAlt) &&
        mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.mainVel)) ||
        mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.mainVel2)) {
        mDetections.main = true;
        mDeploying.main  = true;
        mRecoveryStopDeployAtMillis.main   = timeMs + ROCKET_AVIONIC_MAIN_ACTIVE_TIME_MS;
        digitalWrite(ROCKET_AVIONIC_PIN_MAIN, 1);
    }
}





// "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
void RoutineRocketAvionic::printMotd()
{
    #if PMM_DEBUG
        if (!Serial)
            return;

        PMM_DEBUG_PRINTLN("\n =-=-=-=-=-=-=-=- PMM -=-=-=-=-=-=-=-=-\n\n");

        #if PMM_DATA_LOG_DEBUG
            mPmmModuleDataLog.debugPrintLogHeader();
            PMM_DEBUG_PRINTLN();
        #endif

        #if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED
            PMM_DEBUG_PRINTF("Pmm: Press any key to continue the code. (set PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED (pmmConsts.h) to 0 to disable this!)\n\n");
            for (; !Serial.available(); delay(10));

        #elif PMM_DEBUG_WAIT_AFTER_INIT
            PMM_DEBUG_PRINTF("Pmm: System is halted for %i ms so you can read the init messages.\n\n", PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT)
            delay(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
        #endif

        PMM_DEBUG_PRINTLN("Main loop started!");

    #endif
}


#endif