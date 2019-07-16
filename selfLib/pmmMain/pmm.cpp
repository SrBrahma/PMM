// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include "pmmConsts.h"
#include "pmmExtraCodes.h"
#include "pmmDebug.h"
#include "pmm.h"


Pmm::Pmm() {}



void Pmm::init()
{
    initDebug();     // No need to comment this function. To disable it, change PMM_DEBUG on pmmConsts.h
    runExtraCodes(); // No need to comment this function. To disable it, change PMM_EXTRA_CODES_ENABLE on pmmConsts.h

    // The object mRoutine is specified in the pmm.h automatically, based on the routine defined in the pmmConsts.h .
    mRoutine.init();

}



void Pmm::update()
{
    mRoutine.update();
}



void initDebug()
{
    #if PMM_DEBUG   // Debug
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to its maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print

        #if PMM_DEBUG_TIMEOUT_ENABLED
            uint32_t serialDebugTimeout = millis();
            while (!Serial && ((millis() - serialDebugTimeout) < PMM_DEBUG_TIMEOUT_MILLIS));
        #else
            while (!Serial);
        #endif

        if (Serial)
            debugMorePrintf("Serial initialized!\n");
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