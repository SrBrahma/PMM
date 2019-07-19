// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_PDA

#include <EEPROM.h> // To get the mSessionId

#include "pmmTelemetry/telemetry.h"
#include "pmmSd/sd.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"

// Modules
#include "pmmModules/ports.h"
#include "pmmModules/simpleDataLog/receiver.h"

#include "pmmDebug.h"   // For debug prints

#include "pmmRoutines/pda/pdaConsts.h"
#include "pmmRoutines/pda/pda.h"


RoutinePda::RoutinePda() {}


void RoutinePda::init()
{
    mMainLoopCounter = 0;
    mMillis = millis();

    int initStatus = 0;

    int adrs = 0;
    mSessionId = EEPROM.read(adrs); 
    EEPROM.write(adrs, mSessionId + 1);
    EEPROM.read(adrs);

    // 2) Main objects
    initStatus += mPmmTelemetry.init();
    initStatus += mPmmSd.init(mSessionId);
    initStatus += mPmmGps.init();
    initStatus += mPmmImu.init();

    // 3) Modules
    mSimpleDataLogRx.init(&mPmmSd, mSessionId, PMM_TLM_SIMPLE_DATA_LOG_SOURCE_ADDRESS);
    addVarsSimpleDataLog();
    mPortsReception.addSimpleDataLogRx(&mSimpleDataLogRx);

    // 4) End!
    mMillis = millis(); // Again!
    printMotd();
}

void RoutinePda::update()
{
    if (mPmmTelemetry.updateReception())
        switch(mPmmTelemetry.getRxPacketAllInfoPtr()->port)
            case PORT_ID_SIMPLE_DATA_LOG:
                return mModuleSimpleDataLog->receivedPacket(packetInfo);
            default:;
        }
        return 0;
    }
        mPortsReception.
    mMainLoopCounter++; mMillis = millis();
    advOnlyPrintln();
}



// This MUST be exactly the same, for both transmitter and receiver.
void RoutinePda::addVarsSimpleDataLog()
{
    mSimpleDataLogRx.addBasicInfo        ();
    mSimpleDataLogRx.addAccelerometer    ();
    mSimpleDataLogRx.addGyroscope        ();
    mSimpleDataLogRx.addMpuTemperature   ();
    mSimpleDataLogRx.addBarometerPressure();
    mSimpleDataLogRx.addBarometerAltitude();
    mSimpleDataLogRx.addMagnetometer     ();
    mSimpleDataLogRx.addGpsLatLong       ();
    mSimpleDataLogRx.addGpsAltitude      ();
    mSimpleDataLogRx.addGpsSatellites    ();
}



// "Message of the day" (MOTD). Just a initial text upon the startup, with a optional requirement of a key press.
void RoutinePda::printMotd()
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

        PMM_DEBUG_PRINTLN("Main loop started!");

    #endif
}


#endif