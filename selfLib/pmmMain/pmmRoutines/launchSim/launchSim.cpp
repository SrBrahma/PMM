// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#include <Arduino.h>
#include <generalUnitsOps.h> // for randomDouble()
#include "pmmRoutines/launchSim/launchSim.h"

// LaunchSim(1, 0, 1013.25, 1013.25,
//            5800, 64.2, 85, -28,
//            1000, -10, 1000, -7.7);

LaunchSim::LaunchSim() {}

LaunchSim::LaunchSim(BarometerArg flBarometer, MotorArg flMotor, ParachuteArg flDrogue, ParachuteArg flMain,
                                 float terminalSpeedBothOpened, RoughVerticalDrag drag)
{
    init(flBarometer, flMotor, flDrogue, flMain, terminalSpeedBothOpened, drag);
}

void LaunchSim::init(BarometerArg flBarometer, MotorArg flMotor, ParachuteArg flDrogue, ParachuteArg flMain, float terminalSpeedBothOpened, RoughVerticalDrag drag)
{
    mBarometer                 = flBarometer;
    mMotor.parameters          = flMotor;
    mDrogue.parameters         = flDrogue;
    mMain.parameters           = flMain;
    mDrogueAndMainTerminalVel  = terminalSpeedBothOpened;
    mDrag                      = drag;

    reset();
}

void LaunchSim::reset()
{
    mLastMillis             = millis();
    mMotor.hadIgnited = mMotor.isActive = mMotor.hasLanded = false;
    mMotor.willStopAtMillis = 0;
    
    mAltitude               = mBarometer.initialAltitude;
    mVerticalVelocity       = mVerticalAcceleration = 0;

    mDrogue.state           = ParachuteState::Unopened;
    mDrogue.millisNextState = 0;

    mMain.state             = ParachuteState::Unopened;
    mMain.millisNextState   = 0;
}

LaunchSim::Altitudes LaunchSim::getAltitudes(uint32_t timeMillis)
{
    if (mMotor.hadIgnited && !mMotor.hasLanded)
    {
        uint32_t timeDiffMillis  = timeMillis - mLastMillis;
        double   timeDiffSeconds = timeDiffMillis / ((double) 1000);

        mVerticalAcceleration = 0;
        if (mMotor.isActive)
        {
            mVerticalAcceleration = getMotorVerticalAcceleration(timeMillis);
            if (timeMillis >= mMotor.willStopAtMillis)
                mMotor.isActive = false;
        }
        
        mVerticalAcceleration -= mGravity;
        applyDrag(mVerticalVelocity, mVerticalAcceleration);
        applyParachutesChanges(mVerticalVelocity, mVerticalAcceleration, timeMillis);


        mVerticalVelocity += mVerticalAcceleration * timeDiffSeconds;
        mAltitude         += mVerticalVelocity     * timeDiffSeconds;

        //Serial.printf("altIs %f, velIs %f, accIs %f, timeDiffIs %f\n", mAltitude, mVerticalVelocity, mVerticalAcceleration, timeDiffSeconds);
        if (mAltitude <= mBarometer.initialAltitude) {
            if (!mMotor.isActive)
                mMotor.hasLanded      = true;
            mAltitude             = mBarometer.initialAltitude;
            mVerticalVelocity     = 0;
            mVerticalAcceleration = 0;
        }
    }
    
    mLastMillis = timeMillis;
    return {(float)mAltitude, float(mAltitude + getMeasuredAltitude())};
}

void  LaunchSim::applyDrag(double verticalVelocity, double &verticalAcceleration)
{
    if (verticalVelocity <= mDrag.minVerticalVelocity)
        return;

    double calcAcc = (mDrag.xTo3 * pow(verticalVelocity, 3)) + (mDrag.xTo2 * pow(verticalVelocity, 2)) +
                      (mDrag.xTo1 * verticalVelocity) + mDrag.xTo0;
    verticalAcceleration += calcAcc; // the value should already be negative.
}

void  LaunchSim::applyParachutesChanges(double &verticalVelocity, double &verticalAcceleration, uint32_t timeMillis)
{
    // Change parachute state if it's time.
    if (mDrogue.millisNextState && (timeMillis > mDrogue.millisNextState))
        changeParachuteState(mDrogue, timeMillis, false);
    if (mMain.millisNextState && (timeMillis > mMain.millisNextState))
        changeParachuteState(mMain, timeMillis, false);

    // Those 3 are for fully opened parachutes.
    if ((mDrogue.state == ParachuteState::Opened) &&
        ((mMain.state == ParachuteState::Unopened) || (mMain.state == ParachuteState::Deploying))) {
        verticalVelocity     = mDrogue.parameters.terminalVelocity;
        verticalAcceleration = 0; }
    else if ((mMain.state == ParachuteState::Opened) &&
        ((mDrogue.state == ParachuteState::Unopened) || (mDrogue.state == ParachuteState::Deploying))) {
        verticalVelocity     = mMain.parameters.terminalVelocity;
        verticalAcceleration = 0; }
    else if ((mDrogue.state == ParachuteState::Opened) && (mMain.state == ParachuteState::Opened)) {
        verticalVelocity     = mDrogueAndMainTerminalVel;
        verticalAcceleration = 0; }
    
    // So there are none fully opened parachutes.
    else
    {
        // I give up. There is a hard case when both are opening up together. There is no time and no will to work on it.
        // So, the parachutes will only have effect when fully opened. Hey guy from the future, good luck! You will probably
        // have to use more parameters from the parachute.
    }
}

bool  LaunchSim::getHasLanded()
{
    return mMotor.hasLanded;
}

void  LaunchSim::launch(uint32_t timeMillis)
{
    mMotor.hadIgnited = true;
    mMotor.isActive   = true;
    mMotor.willStopAtMillis = mLastMillis + mMotor.parameters.motorPropulsionMillis;
}

void  LaunchSim::openDrogue(uint32_t timeMillis)
{
    if (mDrogue.state == ParachuteState::Unopened)
        changeParachuteState(mDrogue, mLastMillis, false);
}

void  LaunchSim::openMain(uint32_t timeMillis)
{
    if (mMain.state == ParachuteState::Unopened)
        changeParachuteState(mMain, mLastMillis, false);
}

void  LaunchSim::changeParachuteState(Parachute &parachute, uint32_t timeMillis, bool falseIsNextTrueSelects, ParachuteState state)
{
    if (falseIsNextTrueSelects == false) {
        switch(parachute.state)
        {
            case ParachuteState::Unopened:
                parachute.millisNextState = timeMillis + parachute.parameters.millisToDeploy;
                parachute.state           = ParachuteState::Deploying;
                break;
            case ParachuteState::Deploying:
                parachute.millisNextState = timeMillis + parachute.parameters.millisToFullyOpen;
                parachute.state           = ParachuteState::Opening;
                break;
            case ParachuteState::Opening:
                parachute.millisNextState = 0;
                parachute.state           = ParachuteState::Opened;
                break;
            case ParachuteState::Opened:
                break;
        }
    }
    else {
        switch(state)
        {
            case ParachuteState::Unopened:
                parachute.state           = ParachuteState::Unopened;
                break;
            case ParachuteState::Deploying:
                parachute.millisNextState = timeMillis + parachute.parameters.millisToDeploy;
                parachute.state           = ParachuteState::Deploying;
                break;
            case ParachuteState::Opening:
                parachute.millisNextState = timeMillis + parachute.parameters.millisToFullyOpen;
                parachute.state           = ParachuteState::Opening;
                break;
            case ParachuteState::Opened:
                parachute.state           = ParachuteState::Opened;
                break;
        } // end of switch
    }     // end of else
}         // end of function

float LaunchSim::getVerticalVelocity()     { return mVerticalVelocity; }
float LaunchSim::getVerticalAcceleration() { return mVerticalAcceleration; }


float LaunchSim::getMeasuredAltitude()
{
    return randomDouble(-mBarometer.uncertainty, mBarometer.uncertainty);
}

// Hello someone from the future! Improve this!
double LaunchSim::getMotorVerticalAcceleration(uint32_t timeMillis)
{
    return mMotor.parameters.motorAverageAcc;
}