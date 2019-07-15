#ifndef PMM_FAKE_LAUNCH_h
#define PMM_FAKE_LAUNCH_h

#include <stdint.h>

class FakeLaunch
{
public:
    FakeLaunch(float barometerUncertainty, float initialAltitude, float realSeaLevelPressure, float estimatedSeaLevelPressure,
    float motorPropulsionMillis, float motorAverageAcc, float launchDegree, float drogueTerminalVel,
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
    uint32_t mMotorPropulsionMillis;
    uint32_t mRemainingMotorMillis;

    float    mInitialAltitude;
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
    uint32_t dmDrogueMillisFullDeploy;
    float    mMainTerminalVel;
    uint32_t mMainMillisFullDeploy;
};

#endif