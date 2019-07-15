#include <Arduino.h>
#include "pmmRoutines/fakeLaunch/fakeLaunch.h"

// FakeLaunch(1, 0, 1013.25, 1013.25,
//            5800, 64.2, 85, -28,
//            1000, -10, 1000, -7.7);

FakeLaunch::FakeLaunch(Barometer flBarometer, Motor flMotor, Parachute flDrogue, Parachute flMain, float terminalSpeedBothOpened);
{
    mBarometerUncertainty      = barometerUncertainty;
    mInitialAltitude           = initialAltitude;
    mRealSeaLevelPressure      = realSeaLevelPressure;
    mEstimatedSeaLevelPressure = estimatedSeaLevelPressure;

    mMotorPropulsionTime       = motorPropulsionMillis;
    mMotorAverageAcc           = motorAverageAcc;

    mLaunchDegree              = launchDegree;

    mDrogueTerminalVel         = drogueTerminalVel;
    mDrogueMillisFullDeploy    = drogueMillisFullDeploy;
    mMainTerminalVel           = mainTerminalVel;
    mMainMillisFullDeploy      = mainMillisFullDeploy;
    mDrogueAndMainTerminalVel  = drogueAndMainTerminalVel;
    init();
}

void FakeLaunch::init()
{
    mLastMillis            = millis();
    mHasLaunched           = false;
    mMotorWillStopAtMillis = 0;
    mMotorIsActive         = false;

    mAltitude              = mInitialAltitude;
    mVerticalVelocity      = 0;
    mVerticalAcceleration  = 0;

    mDrogueIsOpened  = false;
    mDrogueIsOpening = false;

    mMainIsOpened   = false;
    mMainIsOpening  = false;

    mDrogueWillDeployAtMillis = 0;
    mMainWillDeployAtMillis   = 0;
}

FakeLaunch::Altitudes FakeLaunch::getAltitudes(uint32_t timeMillis)
{
    if (mHasLaunched)
    {
        uint32_t timeDiffMillis  = mLastMillis - timeMillis;
        double   timeDiffSeconds = timeDiffMillis / ((double) 1000);

        if (mMotorIsActive)
        {
            mVerticalAcceleration = mMotorAverageAcc;
            if (timeMillis >= mMotorWillStopAtMillis)
                mMotorIsActive = false;
        }
        
        if (mDrogueIsOpening)
        if (mDrogueWillDeployAtMillis)
        {
            if (
        }






        mVerticalVelocity += mVerticalAcceleration / timeDiffSeconds;
        mAltitude         += mVerticalVelocity     / timeDiffSeconds;

        if (mAltitude <= mInitialAltitude) {
            mAltitude             = mInitialAltitude;
            mVerticalVelocity     = 0;
            mVerticalAcceleration = 0;
        }
    }
    
    mLastMillis = timeMillis;
    return {mAltitude, getMeasuredAltitude()};
}

float FakeLaunch::getMeasuredAltitude()
{
    return random(-mBarometerUncertainty, mBarometerUncertainty);
}

void  FakeLaunch::launch()
{
    mHasLaunched = true;
    mMotorIsActive = true;
    mMotorWillStopAtMillis    = mLastMillis + mMotorPropulsionTime;
}

void  FakeLaunch::openDrogue()
{
    mDrogueWillDeployAtMillis = mLastMillis + mDrogueMillisFullDeploy;
}

void  FakeLaunch::openMain()
{
    mMainWillDeployAtMillis   = mLastMillis + mMainMillisFullDeploy;
}