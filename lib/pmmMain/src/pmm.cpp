// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include "pmm.h"

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

Pmm::Pmm() {}

int16_t var;

int Pmm::init(bool skipDebugDelay)
{
    mMillis = 0;
    mMainLoopCounter = 0;

    // Debug
    #if PMM_DEBUG
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print
        
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


    PMM_DEBUG_PRINTLN("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=-\n\n");

    mPmmModuleDataLog.debugPrintLogHeader();
    
    PMM_DEBUG_PRINTLN();

    #if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED
        if (Serial)
        {
            PMM_DEBUG_PRINTF("Pmm: Press any key to continue the code. (set PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED (pmmConsts.h) to 0 to disable this!)\n\n");
            for (; !Serial.available(); delay(10));
        }

    #elif PMM_DEBUG && PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT
    {
            PMM_DEBUG_PRINTF("Pmm: System is halted for %i ms so you can read the init messages.\n\n", PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT)
            delay(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
    }
    #endif

    setSystemMode(MODE_DEPLOYED);

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

    #if PMM_DEBUG
        mPmmModuleDataLog.debugPrintLogContent();
        Serial.println();
    #endif


    #if PMM_USE_TELEMETRY
        // This happens here, at "pmm.cpp" and not in the pmmTelemetry, because the PmmPortsXYZ includes the pmmTelemetry, and if pmmTelemetry included the
        // PmmPortzXYZ, that would causa a circular dependency, and the code wouldn't compile. I had the idea to use the address of the functions, but that
        // would make the code a little messy. Give me better alternatives! (but this current alternative isn't THAT bad at all)

        // The Packages objects may/will automatically use the pmmSd and the pmmTelemetry objects.
        if(mPmmTelemetry.updateReception());
            
            //mPmmPortsReception.receivedPacket(mPmmTelemetry.getReceivedPacketArray(), mPmmTelemetry.getReceivedPacketStatusStructPtr());
        //PMM_DEBUG_MORE_PRINTLN("Pmm [M]: Updated Telemetry!");
        if(mPmmTelemetry.updateTransmission());
    #endif



    /*if (packetIDul % 100 == 0)
    {
        Serial.print("timeMsBetween 100 cycles = "); Serial.println(millis() - timePrint);
        timePrint = millis();
    }*/


    mMainLoopCounter++;
}



int Pmm::setSystemMode(pmmSystemState systemMode)
{

    mPmmModuleDataLog.setSystemMode(systemMode);
    mPmmImu.setSystemMode(systemMode);

    return 0;
}