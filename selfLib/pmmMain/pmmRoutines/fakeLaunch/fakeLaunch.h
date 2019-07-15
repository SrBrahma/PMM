
// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil


#ifndef PMM_ROUTINE_FAKE_LAUNCH_h
#define PMM_ROUTINE_FAKE_LAUNCH_h


#include "pmmConsts.h"



#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_FAKE_LAUNCH

#include <stdint.h>
#include <measuresAnalyzer.h>

class RoutineFakeLaunch
{

public:

    RoutineFakeLaunch();

    void init();
    void update();

private:

    // "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
    void printMotd();

    FakeLaunch mFakeLaunch;

    MeasuresAnalyzer mAltitudeAnalyzer;

    struct { int liftOff; int drogue; int main;} mAltAnalyzerIndexes;

    bool mHadLiftOff, mOrderedDrogue, mOrderedMain;

    uint32_t recovery0DisableAtMillis;
    uint32_t recovery1DisableAtMillis;

    uint32_t lastAddedBarAtMillis = 0;
};


class FakeLaunch
{
public:
    typedef struct {
        float uncertainty;
        float initialAltitude;
        float realSeaLevelPressure;
        float estimatedSeaLevelPressure;
    } Barometer;

    typedef struct {
        uint32_t motorPropulsionMillis;
        float    motorAverageAcc;
        float    launchDegree;
    } Motor;

    typedef struct {
        float    terminalVelocity;
        uint32_t millisToDeploy;
        uint32_t millisFullyOpen;
    } Parachute;



    FakeLaunch(Barometer flBarometer, Motor flMotor, Parachute flDrogue, Parachute flMain, float terminalSpeedBothOpened);
    void  init();


    typedef struct {
        float realAltitude;
        float measuredAltitude;
    } Altitudes;


    Altitudes getAltitudes(uint32_t timeMillis);

    // Those 3 below assumes the millis as the last millis entered by getAltitudes(millis).
    void  launch();
    void  openDrogue();
    void  openMain();

private:

    enum class ParachuteState {Unopened, Deploying, Opening, Opened};

    float    getMeasuredAltitude();

    uint32_t mLastMillis;
    bool     mHasLaunched;
    bool     mMotorIsActive;
    uint32_t mMotorWillStopAtMillis;

    double   mAltitude;
    double   mVerticalVelocity;
    double   mVerticalAcceleration;  

    bool     mDrogueIsOpened;
    bool     mDrogueIsOpening;
    bool 
    uint32_t mDrogueWillDeployAtMillis;

    bool     mMainIsOpened;
    bool     mMainIsOpening;
    uint32_t mMainMillisNextState;

    float    mDrogueAndMainTerminalVel;
};

#endif