/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include "pmmConsts.h"
#include "pmmRoutines/rocketAvionic/rocketAvionic.h"

void initDebug();
void printMotd();

class Pmm
{
public:

    Pmm();

    void init();
    void update();

private:
    #if   PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC
        RoutineRocketAvionic mRoutineRocketAvionic;
    #elif PMM_SYSTEM_ROUTINE == PMM_ROUTINE_PDA

    #else
    #    error [PMM] No routine defined. Compilation failed.
    #endif
};

#endif
