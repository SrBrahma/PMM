// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

// Used this Google Sheet to get some values:
// https://docs.google.com/spreadsheets/d/1BrUbO462GLRMjn20ew3LQvr-gB6CcFqa7tsiXAkI2GM/edit?usp=sharing

// All vertical values are positive when going upwards. Negatives are falling!

// WARNING: AT THIS POINT, ALMOST EVERYTHING HERE IS A VERY ROUGH APROXIMATION.

#ifndef PMM_ROUTINE_LAUNCH_SIM_h
#define PMM_ROUTINE_LAUNCH_SIM_h

#include <stdint.h>


class LaunchSim
{
public:
    typedef struct {
        float    uncertainty;               // In meters.
        float    initialAltitude;
        float    realSeaLevelPressure;      // Unused
        float    estimatedSeaLevelPressure; // Unused
    } BarometerArg;

    typedef struct {
        uint32_t motorPropulsionMillis;
        float    motorAverageAcc;
        float    launchDegree;      // Unused
    } MotorArg;

    typedef struct {
        float    terminalVelocity;
        uint32_t millisToDeploy;    // Time after sending signal to open chute to deploy it.
        uint32_t millisToFullyOpen;   // Time after deployed to achieve terminal velocity. Velocity will keep decreasing until terminal vel.
    } ParachuteArg;


    // This is an vertical acceleration third degree polynomial function, where x is the vertical velocity.
    // Here is how I got mine values:
    // Got a .csv from openRocket, opened it on Google Sheets, plotted a graph where X was the vertical velocity, and Y,
    // the acceleration. I only used the values after the motor burnout, and before the apogee. You will get a 2nd/3rd degree like
    // polynomial. Search on Google how to make a linear regression on a Sheets graph. It's very simple. Take the values and use them.
    // You should add ~9.81 (Earth G) to the component without the X, to remove the gravity. You should also add a minimum vertical velocity,
    // as this polynomial can react wrongly to small or negative values. 50 (m/s) is a nice value, as the drag won't do much to the
    // vertical acceleration.
    typedef struct {
        double   xTo3;
        double   xTo2;
        double   xTo1;
        double   xTo0;
        double   minVerticalVelocity;
    } RoughVerticalDrag;

    // Call init after, if using empty constructor!! YOU MUST DO IT!
    LaunchSim();

    // No need to call init if using this constructor. YOU DONT NEED TO!
    LaunchSim(BarometerArg flBarometer, MotorArg flMotor, ParachuteArg flDrogue, ParachuteArg flMain, float terminalSpeedBothOpened, RoughVerticalDrag drag);
    
    void  init(BarometerArg flBarometer, MotorArg flMotor, ParachuteArg flDrogue, ParachuteArg flMain, float terminalSpeedBothOpened, RoughVerticalDrag drag);
    
    // Resets all the runtime variables. Aka reset the flight.
    void  reset();


    typedef struct {
        float realAltitude;
        float measuredAltitude;
    } Altitudes;
    Altitudes getAltitudes(uint32_t timeMs);

    float getVerticalVelocity();
    float getVerticalAcceleration();

    // Those 3 below assumes the millis as the last millis entered by getAltitudes(millis).
    void  launch(uint32_t timeMs);
    void  openDrogue(uint32_t timeMs);
    void  openMain(uint32_t timeMs);
    bool  getHasLanded();

private:
    enum class ParachuteState {Unopened, Deploying, Opening, Opened};
    typedef struct {
        ParachuteArg   parameters;
        ParachuteState state;
        uint32_t       millisNextState;
    } Parachute;

    struct {
        MotorArg parameters;
        bool     hadIgnited;
        bool     hasLanded;
        bool     isActive;
        uint32_t willStopAtMillis;
    } mMotor;

    float    getMeasuredAltitude();

    double getMotorVerticalAcceleration(uint32_t timeMs);

    void     applyDrag(double verticalVelocity, double &verticalAcceleration);
    void     applyParachutesChanges(double &verticalVelocity, double &verticalAcceleration, uint32_t timeMs);
    //
    void     changeParachuteState(Parachute &parachute, uint32_t timeMs, bool falseIsNextTrueSelects, ParachuteState state = ParachuteState::Unopened);

    RoughVerticalDrag  mDrag;
    BarometerArg       mBarometer;

    Parachute  mDrogue;
    Parachute  mMain;

    uint32_t   mLastMillis;


    double     mAltitude;
    double     mVerticalVelocity;
    double     mVerticalAcceleration;

    float      mDrogueAndMainTerminalVel;

    double     mGravity = 9.807;
};

#endif