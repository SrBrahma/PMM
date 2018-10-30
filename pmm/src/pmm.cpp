// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include <pmm.h>


#include <pmmConsts.h>
#include "pmmImu/pmmImu.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"
#include "pmmHealthBasicSignals/pmmHealthBasicSignals.h"



#if PMM_USE_TELEMETRY
    #include "pmmTelemetry/pmmTelemetry.h"
#endif

#if PMM_USE_GPS
    #include "pmmGps/pmmGps.h"
#endif

#if PMM_USE_SD
    #include "pmmSd/pmmSd.h"
#endif

#include "pmmEeprom/pmmEeprom.h"

// Packages
#include "pmmPackages/dataLog/dataLog.h"
#include "pmmPackages/messageLog/messageLog.h"
#include "pmmPackages/portsReception.h"



void Pmm::init()
{
    // Debug
    #if PMM_DEBUG_SERIAL
        uint32_t serialDebugTimeout = millis();
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print
        Serial.println("Pmm: Serial initialized!");
        #if PMM_DEBUG_SERIAL_TIMEOUT_ENABLED
            while (!Serial && (millis() - serialDebugTimeout < PMM_DEBUG_SERIAL_TIMEOUT_MILLIS));        // wait for serial port to connect. Needed for native USB port only

        #else
            while (!Serial);

        #endif

        if (Serial)
            PMM_DEBUG_PRINT("Serial initialized!");
    #endif


    mPmmErrorsCentral.init(&mPackageLogId);

    // Telemetry ====================================================================================
    #if PMM_USE_TELEMETRY
        mPmmTelemetry.init(&mPmmErrorsCentral);
    #endif

    // Packages =====================================================================================
    // PmmPackageDataLog
    mPmmPackageDataLog.init(&mPmmTelemetry);
    mPmmPackageDataLog.addPackageBasicInfo(&mPackageLogId, &mPackageTimeMs);

    // PmmPackageMessageLog
    mPmmPackageMessageLog.init(&mPackageLogId, &mPackageTimeMs, &mPmmTelemetry, &mPmmSd);

    // PmmPortsReception
    mPmmPortsReception.init(&mPmmPackageDataLog, &mPmmPackageMessageLog);


    // GPS ==========================================================================================
    #if PMM_USE_GPS
        mPmmGps.init(&mPmmErrorsCentral);
        mPmmPackageDataLog.addGps(mPmmGps.getGpsStructPtr());
    #endif


    // SD ===========================================================================================
    #if PMM_USE_SD
        mPmmSd.init(&mPmmErrorsCentral);
    #endif


    // IMU ==========================================================================================
    #if PMM_USE_IMU
        mPmmImu.init(&mPmmErrorsCentral);
        mPmmPackageDataLog.addImu(mPmmImu.getImuStructPtr());
    #endif

    
    mPackageLogId = 0;
    mPackageTimeMs = 0;


    PMM_DEBUG_PRINT("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=- \n");
    mPmmPackageDataLog.debugPrintLogHeader();
}




// Where EVERYTHING happens!
void Pmm::update()
{
    //PMM_DEBUG_PRINT_MORE("Pmm [M]: Looped!");
    //PMM_DEBUG_PRINT(i++);


    mPackageTimeMs = millis();                  // Packet time, in miliseconds. (unsigned long)



    #if PMM_USE_IMU
        mPmmImu.update();
        //PMM_DEBUG_PRINT_MORE("Pmm [M]: Updated Imu!");
    #endif



    #if PMM_USE_GPS
        mPmmGps.update();
        //PMM_DEBUG_PRINT_MORE(Pmm [M]: Updated Gps!");
    #endif



    #if PMM_USE_SD
    #endif



    #if PMM_DEBUG_SERIAL
        mPmmPackageDataLog.debugPrintLogContent();
        Serial.println();
    #endif



    #if PMM_USE_TELEMETRY
        // This happens here, at "pmm.cpp" and not in the pmmTelemetry, because the PmmPortsXYZ includes the pmmTelemetry, and if pmmTelemetry included the
        // PmmPortzXYZ, that would causa a circular dependency, and the code wouldn't compile. I had the idea to use the address of the functions, but that
        // would make the code a little messy. Give me better alternatives! (but this current alternative isn't THAT bad at all)

        // The Packages objects may/will automatically use the pmmSd and the pmmTelemetry objects.
        if(mPmmTelemetry.updateReception());
            mPmmPortsReception.receivedPacket(mPmmTelemetry.getReceivedPacketArray(), mPmmTelemetry.getReceivedPacketStatusStructPtr());
        //PMM_DEBUG_PRINT_MORE("Pmm [M]: Updated Telemetry!");
        if(mPmmTelemetry.updateTransmission());
    #endif



    /*if (packetIDul % 100 == 0)
    {
        Serial.print("timeMsBetween 100 cycles = "); Serial.println(millis() - timePrint);
        timePrint = millis();
    }*/

        //mPmmErrorsCentral.updateLedsAndBuzzer();
    mPackageLogId ++;
}
