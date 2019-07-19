/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include "pmmConsts.h"
#include "pmmRoutines/rocketAvionic/rocketAvionic.h"
#include "pmmRoutines/launchSim/launchSimRoutine.h"

void initDebug();
void printMotd();

class Pmm
{
public:
    Pmm();

    void init();
    void update();

private:
    #if   PMM_SYSTEM_ROUTINE  ==  PMM_ROUTINE_ROCKET_AVIONIC
        RoutineRocketAvionic   mRoutine;
    #elif PMM_SYSTEM_ROUTINE  ==  PMM_ROUTINE_PDA
    #elif PMM_SYSTEM_ROUTINE  ==  PMM_ROUTINE_LAUNCH_SIM
        RoutineLaunchSim mRoutine;
    #else
    #    error [PMM] No routine defined. Compilation failed.
    #endif
};

#endif
