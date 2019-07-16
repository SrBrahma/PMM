#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_LAUNCH_SIM

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
}


void RoutineLaunchSim::init()
{
    mLaunchSim.reset();
    recovery0DisableAtMillis = 0;
    recovery1DisableAtMillis = 0;
    printMotd();
    Serial.printf("Time(s),Altitude(m),VertVelocity(m/s),VertAcceleration(m/s^2)\n");
}

int i = 0; uint32_t millis2 = 0;
void RoutineLaunchSim::update()
{
    if (i == 10)
        mLaunchSim.launch(millis2);

    if (i < 1000)
    {
        Serial.printf("%f,%f,%f,%f\n", mLaunchSim.getAltitudes(millis2).realAltitude, (float)millis2/1000,
                                 mLaunchSim.getVerticalVelocity(), mLaunchSim.getVerticalAcceleration());
    }
    millis2 += PMM_LAUNCH_SIMULATOR_INTERVAL_MS;
    i++;
    //delay(PMM_LAUNCH_SIMULATOR_DELAY_MS);
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