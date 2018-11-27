// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include "pmm.h"

#include "pmmConsts.h"

#include "pmmErrorsCentral/pmmErrorsCentral.h"
#include "pmmHealthBasicSignals/pmmHealthBasicSignals.h"

#include "pmmEeprom/pmmEeprom.h"

#if PMM_USE_TELEMETRY
    #include "pmmTelemetry/pmmTelemetry.h"
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



// Packages
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"
#include "pmmModules/portsReception.h"



Pmm::Pmm()
    :
    // SD ===========================================================================================
    #if PMM_USE_SD
        mPmmSd(mSessionId)
    #endif
{

    mMillis = 0;
    mLoopId = 0;

  // Debug
    #if PMM_DEBUG
        uint32_t serialDebugTimeout = millis();
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print
        
        #if PMM_DEBUG_TIMEOUT_ENABLED
            while (!Serial && (millis() - serialDebugTimeout < PMM_DEBUG_TIMEOUT_MILLIS));
        
        #else
            while (!Serial);

        #endif

        if (Serial)
            PMM_DEBUG_PRINTLN_MORE("Pmm [M]: Serial initialized!");
    #endif


    mPmmErrorsCentral.init(&mLoopId);


  // Telemetry ====================================================================================
    #if PMM_USE_TELEMETRY
        mPmmTelemetry.init();
    #endif





  // GPS ==========================================================================================
    #if PMM_USE_GPS
        mPmmGps.init();
        mPmmModuleDataLog.addGps(mPmmGps.getGpsStructPtr());
    #endif


  // IMU ==========================================================================================
    #if PMM_USE_IMU
        mPmmImu.init();
        mPmmModuleDataLog.addImu(mPmmImu.getImuStructPtr());
    #endif


  // Packages =====================================================================================

    // PmmModuleDataLog
    mPmmModuleDataLog.init(&mPmmTelemetry, &mPmmSd, &mSessionId, &mLoopId, &mMillis);

        #if PMM_USE_GPS
            mPmmModuleDataLog.addGps(mPmmGps.getGpsStructPtr());
        #endif
        
        #if PMM_USE_IMU
            mPmmModuleDataLog.addImu(mPmmImu.getImuStructPtr());
        #endif

    // PmmModuleMessageLog
    mPmmModuleMessageLog.init(&mLoopId, &mMillis, &mPmmTelemetry, &mPmmSd);

    // PmmPortsReception
    mPmmPortsReception.init(&mPmmModuleDataLog, &mPmmModuleMessageLog);


    PMM_DEBUG_PRINTLN("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=- \n");
    mPmmModuleDataLog.debugPrintLogHeader();

    #if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED
        if (Serial)
        {
            Serial.print("\nPmm: Press any key to continue the code. (set PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED (pmmConsts.h) to 0 to disable this!)\n");
            for (;!Serial.available();delay(10));
        }
    #elif PMM_DEBUG && PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT
        if (Serial)
        {
            Serial.print("\nPmm: System is halted for ");
            Serial.print(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
            Serial.println(" ms so you can read the init messages.");
            delay(PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT);
        }
    #endif
}




// Where EVERYTHING happens!
void Pmm::update()
{

    mMillis = millis();



    #if PMM_USE_IMU
        mPmmImu.update();
        //PMM_DEBUG_PRINTLN_MORE("Pmm [M]: Updated Imu!");
    #endif



    #if PMM_USE_GPS
        mPmmGps.update();
        //PMM_DEBUG_PRINTLN_MORE(Pmm [M]: Updated Gps!");
    #endif



    #if PMM_USE_SD
    #endif



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
        //PMM_DEBUG_PRINTLN_MORE("Pmm [M]: Updated Telemetry!");
        if(mPmmTelemetry.updateTransmission());
    #endif



    /*if (packetIDul % 100 == 0)
    {
        Serial.print("timeMsBetween 100 cycles = "); Serial.println(millis() - timePrint);
        timePrint = millis();
    }*/

        //mPmmErrorsCentral.updateLedsAndBuzzer();
    mLoopId ++;
    delay(500);
}
