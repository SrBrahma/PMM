// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include "pmmConsts.h"
#include "pmmExtraCodes.h"
#include "pmm.h"


Pmm::Pmm() {}



void Pmm::init()
{
    initDebug();     // No need to comment this function. To disable it, change PMM_DEBUG on pmmConsts.h
    runExtraCodes(); // No need to comment this function. To disable it, change PMM_EXTRA_CODES_ENABLE on pmmConsts.h

    #if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC
    mRoutineRocketAvionic.init();

    #elif PMM_SYSTEM_ROUTINE == PMM_ROUTINE_PDA

    #endif
}



void Pmm::update()
{
    #if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC
    mRoutineRocketAvionic.update();

    #elif PMM_SYSTEM_ROUTINE == PMM_ROUTINE_PDA

    #endif
}



void initDebug()
{
    #if PMM_DEBUG   // Debug
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to its maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print

        #if PMM_DEBUG_TIMEOUT_ENABLED
            uint32_t serialDebugTimeout = millis();
            while (!skipDebugDelay && !Serial && (millis() - serialDebugTimeout < PMM_DEBUG_TIMEOUT_MILLIS));
        #else
            while (!Serial);
        #endif

        if (Serial)
            debugMorePrintf("Serial initialized!\n");
    #endif
}

void printMotd() // "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
{
    #if PMM_DEBUG
        if (!Serial)
            return;

        PMM_DEBUG_PRINTLN("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=-\n\n");

        #if PMM_DATA_LOG_DEBUG
            mPmmModuleDataLog.debugPrintLogHeader();
            PMM_DEBUG_PRINTLN();
        #endif

        #if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED
            PMM_DEBUG_PRINTF("Pmm: Press any key to continue the code. (set PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED (pmmConsts.h) to 0 to disable this!)\n\n");
            for (; !Serial.available(); delay(10));

        #elif PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT
            PMM_DEBUG_PRINTF("Pmm: System is halted for %i ms so you can read the init messages.\n\n", PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT)
            delay(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
        #endif

        PMM_DEBUG_PRINTLN("Main loop started!");

    #endif
}





// These below are no longer used, for now.
// // Be careful using the functions below. You can easily turn the PMM code into a hell by bypassing the default running order.
// PmmImu*              Pmm::getPmmImuPtr()            { return &mPmmImu;               }
// PmmTelemetry*        Pmm::getTelemetryPtr()         { return &mPmmTelemetry;         }
// PmmGps*              Pmm::getGpsPtr()               { return &mPmmGps;               }
// PmmSd*               Pmm::getSdPtr()                { return &mPmmSd;                }
// PmmModuleDataLog*    Pmm::getModuleDataLog()        { return &mPmmModuleDataLog;     }
// PmmModuleMessageLog* Pmm::getModuleMessageLog()     { return &mPmmModuleMessageLog;  }