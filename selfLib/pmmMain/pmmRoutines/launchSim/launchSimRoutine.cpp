#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_LAUNCH_SIM

#include <generalUnitsOps.h>
#include "pmmDebug.h"
#include "pmmRoutines/launchSim/launchSim.h"
#include "pmmRoutines/launchSim/launchSimRoutine.h"


RoutineLaunchSim::RoutineLaunchSim()
{
    LaunchSim::BarometerArg      bar    = {1, 0, 1013.25, 1013.25};
    LaunchSim::MotorArg          motor  = {5800, 70.0, 85.0};
    LaunchSim::ParachuteArg      drogue = {-27, 500, 1000};
    LaunchSim::ParachuteArg      main   = {-10, 500, 1000};
    LaunchSim::RoughVerticalDrag drag   = {-8.7e-7, 8.51e-5, -0.02, -9.32 + 9.81};

    mLaunchSim.init(bar, motor, drogue, main, -10, drag);

    if (mAltitudeAnalyzer.init(millisToMicros(PMM_LAUNCH_SIMULATOR_INTERVAL_MS) * 0.9, millisToMicros(100), secondsToMicros(1),
         10))//, true, 1, 0.1))
        advPrintf("Fatal error! Failed to alloc memory to mAltitudeAnalyzer!");
    mAltAnalyzerIndexes.liftOff = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreGreaterThan, 5, MeasuresAnalyzer::Time::Second);
    mAltAnalyzerIndexes.drogue  = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreLesserThan, -5, MeasuresAnalyzer::Time::Second);
    mAltAnalyzerIndexes.mainAlt = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::Values,
                                    MeasuresAnalyzer::Relation::AreLesserThan, 650);
    mAltAnalyzerIndexes.mainVel = mAltitudeAnalyzer.addCondition(95, MeasuresAnalyzer::CheckType::FirstDerivative,
                                    MeasuresAnalyzer::Relation::AreLesserThan, -5, MeasuresAnalyzer::Time::Second);
}


void RoutineLaunchSim::init()
{
    mLaunchSim.reset();

    mDetections.liftOff = mDetections.drogue = mDetections.main = false;
    mDeploying.drogue   = mDeploying.main    = false;
    mRecoveryStopDeployAtMillis.drogue = mRecoveryStopDeployAtMillis.main = 0;

    mTimeMs      = 0;
    mMainLoopCounter = 0;

    printMotd();
    Serial.printf("Time(s),Altitude(m),MeasuredAltitude(m),VertVelocity(m/s),VertAcceleration(m/s^2),detectedLiftOff,detectedDrogue,detectedMain\n");
}


void RoutineLaunchSim::update()
{
    disableRecDeployIfTimePassed(mTimeMs);

    if (mMainLoopCounter == 10) // Wait just a little bit before the launch.
        mLaunchSim.launch(mTimeMs);


    LaunchSim::Altitudes altitude = mLaunchSim.getAltitudes(mTimeMs);


    deployRecoveriesIfConditionsMet(mTimeMs, altitude.measuredAltitude);

    // VsCode/PlatformIO has a max terminal scroll length of a little more than 1000 lines.
    // So, as we don't need every point of the simulation, we output the results at a multiplus of X (10, here).
    // You can change those 2 '10' to get a wider min time. At '1000', it generates ~14 hours of test,
    // BUT TAKES LESS THAN A MINUTE TO SIMULATE IT!
    if ((mMainLoopCounter % 10 == 0) && ((mMainLoopCounter / 10) < 1000) && !mLaunchSim.getHasLanded())
    {
        Serial.printf("%f,%f,%f,%f,%f,%s,%s,%s\n", mTimeMs / 1000.0, altitude.realAltitude, altitude.measuredAltitude,
                      mLaunchSim.getVerticalVelocity(), mLaunchSim.getVerticalAcceleration(),
                      mDetections.liftOff?"1":"", mDetections.drogue?"50":"", mDetections.main?"100":"");
    }

    mTimeMs += PMM_LAUNCH_SIMULATOR_INTERVAL_MS;
    mMainLoopCounter++;
 
}

void RoutineLaunchSim::disableRecDeployIfTimePassed(uint32_t timeMs)
{
    if (mRecoveryStopDeployAtMillis.drogue && (timeMs > mRecoveryStopDeployAtMillis.drogue)) {
        mRecoveryStopDeployAtMillis.drogue = 0;
        mDeploying.drogue = false;
        digitalWrite(34, 0);
    }
    if (mRecoveryStopDeployAtMillis.main   && (timeMs > mRecoveryStopDeployAtMillis.main  )) {
        mRecoveryStopDeployAtMillis.main   = 0;
        mDeploying.main   = false;
        digitalWrite(35, 0);
    }
}



void RoutineLaunchSim::deployRecoveriesIfConditionsMet(uint32_t timeMs, float altitude)
{
    mAltitudeAnalyzer.addMeasure(altitude, millisToMicros(timeMs));
    if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.liftOff)) {
        mDetections.liftOff = true;
    }
    if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.drogue))  {
        mDetections.drogue = true;
        mDeploying.drogue  = true;
        mRecoveryStopDeployAtMillis.drogue = timeMs + 1000;
        //digitalWrite(34, 1);
        mLaunchSim.openDrogue(timeMs);
    }
    if (mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.mainAlt) &&
        mAltitudeAnalyzer.checkCondition(mAltAnalyzerIndexes.mainVel))    {
        mDetections.main = true;
        mDeploying.main  = true;
        mRecoveryStopDeployAtMillis.main   = timeMs + 1000;
        //digitalWrite(35, 1);
        //mLaunchSim.openMain(timeMs);
    }
}



// "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
void RoutineLaunchSim::printMotd()
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

        Serial.println();
    #endif
}


#endif