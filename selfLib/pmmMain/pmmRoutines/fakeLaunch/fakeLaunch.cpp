#include <Arduino.h>
#include "pmmRoutines/fakeLaunch/fakeLaunch.h"

FakeLaunch::FakeLaunch(float barometerUncertainty, float initialAltitude, float realSeaLevelPressure, float estimatedSeaLevelPressure,
    float motorPropulsionMillis, float motorAverageAcc, float launchDegree, float drogueTerminalVel,
    uint32_t drogueMillisFullDeploy, float mainTerminalVel, uint32_t mainMillisFullDeploy)
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
    init();
}

void FakeLaunch::init()
{
    mLastMillis           = millis();
    mHasLaunched          = false;
    mRemainingMotorMillis = mMotorPropulsionMillis;

    mAltitude             = mInitialAltitude;
    mVerticalVelocity     = 0;
    mVerticalAcceleration = 0;  

    mBarometerUncertainty;
    mInitialAltitude;
    mRealSeaLevelPressure;
    mEstimatedSeaLevelPressure;
    mMotorPropulsionTime;
    mMotorAverageAcc;
    mLaunchDegree;
    mDrogueTerminalVel;
    mDrogueMillisFullDeploy;
    mMainTerminalVel;
    mMainMillisFullDeploy;
}

{
public:
    FakeLaunch(float barometerUncertainty, float initialAltitude, float realSeaLevelPressure, float estimatedSeaLevelPressure,
    float motorPropulsionTime, float motorAverageAcc, float launchDegree, float drogueTerminalVel,
    uint32_t drogueMillisFullDeploy, float mainTerminalVel, uint32_t mainMillisFullDeploy);

    void  init();
    float getRealAltitude(uint32_t timeMillis);
    float getMeasuredAltitude(uint32_t timeMillis);

    void  launch();
    void  openDrogue(uint32_t timeToFullyOpen);
    void  openMain(uint32_t timeToFullyOpen);

private:
    uint32_t mLastMillis;
    bool     mHasLaunched;
    uint32_t mRemainingMotorTime;

    double   mAltitude;
    double   mVerticalVelocity;
    double   mVerticalAcceleration;  

    float    mBarometerUncertainty;
    float    mInitialAltitude;
    float    mRealSeaLevelPressure;
    float    mEstimatedSeaLevelPressure;
    float    mMotorPropulsionTime;
    float    mMotorAverageAcc;
    float    mLaunchDegree;
    float    mDrogueTerminalVel;
    uint32_t mDrogueMillisFullDeploy;
    float    mMainTerminalVel;
    uint32_t mMainMillisFullDeploy;
};