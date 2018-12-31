// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include "pmmConsts.h"

#include "pmmHealthBasicSignals/pmmHealthBasicSignals.h"

#include "pmmEeprom/pmmEeprom.h"

#if PMM_USE_TELEMETRY
    #include "pmmTelemetry/pmmTelemetry.h"
    #include "pmmModules/portsReception.h"
#endif

#if PMM_USE_SD
    #include "pmmSd/pmmSd.h"
#endif

#if PMM_USE_IMU
    #include "pmmImu/pmmImu.h"
#endif

#if PMM_USE_GPS
    #include "pmmGps/pmmGps.h"
#endif

// Modules
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"

#include "pmmDebug.h"   // For debug prints

#include "pmm.h"



Pmm::Pmm() {}



int Pmm::init(bool skipDebugDelay)
{
    mMillis = 0;
    mMainLoopCounter = 0;

    mSessionId = 0x0; // Later, use the EEPROM.
    // Debug
    #if PMM_DEBUG
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



    // Telemetry ====================================================================================
    #if PMM_USE_TELEMETRY
        mPmmTelemetry.init();
    #endif


    // SD ===========================================================================================
    #if PMM_USE_SD
        mPmmSd.init(mSessionId);
    #endif


    // GPS ==========================================================================================
    #if PMM_USE_GPS
        mPmmGps.init();
    #endif


    // IMU ==========================================================================================
    #if PMM_USE_IMU
        mPmmImu.init();
    #endif



    // PmmModuleDataLog
    mPmmModuleDataLog.init(&mPmmTelemetry, &mPmmSd, mSessionId, 0, &mMainLoopCounter, &mMillis);

        #if PMM_USE_GPS
            mPmmModuleDataLog.addGps(mPmmGps.getGpsStructPtr());
        #endif
        
        #if PMM_USE_IMU
            mPmmModuleDataLog.addImu(mPmmImu.getImuStructPtr());
        #endif

    // PmmModuleMessageLog
    mPmmModuleMessageLog.init(&mMainLoopCounter, &mMillis, &mPmmTelemetry, &mPmmSd);

    // PmmPortsReception
    mPmmPortsReception.init(&mPmmModuleDataLog, &mPmmModuleMessageLog);



    
    #if PMM_DEBUG
        PMM_DEBUG_PRINTLN("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=-\n\n");

        #if PMM_DATA_LOG_DEBUG
            mPmmModuleDataLog.debugPrintLogHeader();
            PMM_DEBUG_PRINTLN();
        #endif

        #if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED
            if (Serial)
            {
                PMM_DEBUG_PRINTF("Pmm: Press any key to continue the code. (set PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED (pmmConsts.h) to 0 to disable this!)\n\n");
                for (; !Serial.available(); delay(10));
            }

        #elif PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT
        {
            if (Serial)
            {
                PMM_DEBUG_PRINTF("Pmm: System is halted for %i ms so you can read the init messages.\n\n", PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT)
                delay(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
            }
        }
        #endif
    #endif

    setSystemMode(MODE_DEPLOYED);

    PMM_DEBUG_PRINTLN("Main loop started!");
    return 0;
}




// Where EVERYTHING happens!
void Pmm::update()
{

    mMillis = millis();

    #if PMM_USE_IMU
        mPmmImu.update();
    #endif


    #if PMM_USE_GPS
        mPmmGps.update();
    #endif


    mPmmModuleDataLog.update();

    #if PMM_DEBUG && PMM_DATA_LOG_DEBUG
        mPmmModuleDataLog.debugPrintLogContent();
        Serial.println();
    #endif


    #if PMM_USE_TELEMETRY
        // This happens here, at "pmm.cpp" and not in the pmmTelemetry, because the PmmPortsXYZ includes the pmmTelemetry, and if pmmTelemetry
        // included the PmmPortzXYZ, that would causa a circular dependency, and the code wouldn't compile.
        if(mPmmTelemetry.updateReception())
            mPmmPortsReception.receivedPacket(mPmmTelemetry.getReceivedPacketAllInfoStructPtr());
        mPmmTelemetry.updateTransmission();

    #endif



    /*if (mMainLoopCounter % 100 == 0)
    {
        debugMorePrintf("Time between 100 cycles = millis() - timePrint")
        timePrint = millis();
    }*/


    mMainLoopCounter++;
    // delay(500);
}



int Pmm::setSystemMode(pmmSystemState systemMode)
{
    mPmmModuleDataLog.setSystemMode(systemMode);
    mPmmImu.setSystemMode(systemMode);

    return 0;
}