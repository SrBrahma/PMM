// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)


#include <pmm.h>
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmErrorsCentral.h>
#include <pmmHealthSignals.h>



#if PMM_USE_TELEMETRY
    #include <pmmTelemetry.h>
#endif

#if PMM_USE_GPS
    #include <pmmGps.h>
#endif

#if PMM_USE_SD
    #include <pmmSd.h>
#endif

// Packages
#include <packages/pmmPackageLog.h>
#include "packages/pmmPackageString.h"


void Pmm::init()
{
    // Debug
    #if PMM_DEBUG_SERIAL
        unsigned long serialDebugTimeout = millis();
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

    // Telemetry
    #if PMM_USE_TELEMETRY
        mPmmTelemetry.init(&mPmmErrorsCentral);
    #endif


    // PmmPackageLog
    mPmmPackageLog.init(&mPmmTelemetry);
    mPmmPackageLog.addPackageBasicInfo(&mPackageLogId, &mPackageTimeMs);


    // GPS
    #if PMM_USE_GPS
        mPmmGps.init(&mPmmErrorsCentral);
        mPmmPackageLog.addGps(mPmmGps.getGpsStructPtr());
    #endif


    // SD
    #if PMM_USE_SD
        mPmmSd.init(&mPmmErrorsCentral);
    #endif


    // IMU
    #if PMM_USE_IMU
        mPmmImu.init(&mPmmErrorsCentral);
        mPmmPackageLog.addImu(mPmmImu.getImuStructPtr());
    #endif


    mPackageLogId = 0;
    mPackageTimeMs = 0;


    PMM_DEBUG_PRINT("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=- \n");
    mPmmPackageLog.debugPrintLogHeader();
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
        mPmmPackageLog.debugPrintLogContent();
        Serial.println();
    #endif




    //mPmmErrorsCentral.updateLedsAndBuzzer();






    #if PMM_USE_TELEMETRY
        // This happens here, at "pmm.cpp" and not in the pmmTelemetry, because the pmmPackagesXYZ includes the pmmTelemetry, and if pmmTelemetry included the
        // pmmPackagezXYZ, that would causa a circular dependency, and the code wouldn't compile. I had the idea to use the address of the functions, but that
        // would make the code a little messy. Give me better alternatives! (but this current alternative isn't bad at all)

        // They may automatically use the pmmSd and the pmmTelemetry objects.
        switch(mPmmTelemetry.updateReception())
        {
            case PMM_PACKAGE_NONE: // This could be with the default case. But will leave it here for A E S T H E T I C S (or bug reporting if default - invalid type)
                break;
            case PMM_PACKAGE_LOG:
                mPmmPackageLog.receivedPackageLogInfo(mPmmTelemetry.getReceivedPacketArray(), mPmmTelemetry.getReceivedPacketLength());
                break;
            case PMM_PACKAGE_LOG_INFO:
                mPmmPackageLog.receivedPackageLogInfo(mPmmTelemetry.getReceivedPacketArray(), mPmmTelemetry.getReceivedPacketLength());
                break;
            case PMM_PACKAGE_STRING:
                mPmmPackageLog.receivedPackageLogInfo(mPmmTelemetry.getReceivedPacketArray(), mPmmTelemetry.getReceivedPacketLength());
                break;
            case PMM_PACKAGE_REQUEST:
                break;
            default:
                break; // This link is not used here, but a good information about default in switchs: https://softwareengineering.stackexchange.com/a/201786
        }

        mPmmTelemetry.updateTransmission();
        //PMM_DEBUG_PRINT_MORE("Pmm [M]: Updated Telemetry!");
    #endif





    /*if (packetIDul % 100 == 0)
    {
        Serial.print("timeMsBetween 100 cycles = "); Serial.println(millis() - timePrint);
        timePrint = millis();
    }*/




    mPackageLogId ++;
}
