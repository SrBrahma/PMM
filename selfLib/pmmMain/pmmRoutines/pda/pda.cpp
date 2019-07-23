// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_PDA

#include <EEPROM.h> // To get the mSessionId

#include "pmmTelemetry/telemetry.h"
#include "pmmSd/sd.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"

#include "Location.h"

// Modules
#include "pmmModules/ports.h"
#include "pmmModules/simpleDataLog/receiver.h"

#include "LiquidCrystal_I2C.h" // For the 2004 char display

#include "pmmDebug.h"       // For debug prints

#include "pmmRoutines/pda/pdaConsts.h"
#include "pmmRoutines/pda/pda.h"


RoutinePda::RoutinePda() : mLiquidCrystal(PDA_DISPLAY_ADDRESS, PDA_DISPLAY_COLUMNS, PDA_DISPLAY_ROWS) {}


void RoutinePda::init()
{
    mMainLoopCounter = 0;
    mMillis = millis();

    int initStatus = 0;

    // 1) Get and increase the SessionID.
    mSessionId = EEPROM.read(PMM_EEPROM_INDEX_SESSION_ID); 
    EEPROM.write(PMM_EEPROM_INDEX_SESSION_ID, mSessionId + 1);

    // 2) Main objects
    initStatus += mPmmTlm.init();
    initStatus += mPmmSd.init(mSessionId);
    initStatus += mPmmGps.init();
    initStatus += mPmmImu.init();

    // 3) Modules
    mSimpleDataLogRx.init(&mPmmSd, mSessionId, PMM_TLM_SIMPLE_DATA_LOG_SOURCE_ADDRESS);
    addVarsSimpleDataLog();

    // 4) Display
    // mLiquidCrystal.begin();

    // mLiquidCrystal.backlight();
    // mLiquidCrystal.printf("aaaaaaaa!\n");
    mMillis = millis(); // Again!
    printMotd();
}

void RoutinePda::update()
{
    mPmmGps.update(mMillis);
    mPmmImu.update();

    if (updateTelemetryReception() == PORT_ID_SIMPLE_DATA_LOG)  {
        updatePdaData();
        mSimpleDataLogRx.storeOnSd(mSimpleDataLogRx.getSourceSession(), true);
    }

    // Avoids the display updating too much frequently.
    if (millis() > (mLastMillisRefreshedDisplay + PDA_DISPLAY_MIN_TIME_MS_BETWEEN_UPDATES))   {
        updateDisplay();
        mLastMillisRefreshedDisplay = millis();
    }

    mMainLoopCounter++; mMillis = millis();
}



//   12345678901234567890
//  .____________________.
// 1|TxCt 999999 Sess 256|
// 2|MLCt 999999 H 99999m|
// 3|Dist 99999m Bear 360|
// 4|NTxP   999s NRx 999s|
void RoutinePda::updateDisplay()
{
    mLiquidCrystal.home();
    uint32_t TxCt = mTxData.txCounter       % 999999; // It cycles if overflows
    uint32_t MLCt = mTxData.mainLoopCounter % 999999; // It cycles if overflows
    float H = mTxData.barometerAltitude;
    if (H > 99999)
        H = 99999;

    float Dist = mPmmGps.distanceToInMeters(mTxData.gpsLat, mTxData.gpsLon);
    if (Dist > 99999)
        Dist = 99999;

    float Bear = mPmmGps.bearingToInDegrees(mTxData.gpsLat, mTxData.gpsLon) - mPmmImu.getBearingDegree();
    if (Bear < 0)
        Bear = 360 - Bear;
    
    uint32_t NTxP = floor((mTxData.timeMillis - mTxData.lastGpsLocationTimeMs) / 1000.0);
    if (NTxP > 999)
        NTxP = 999;

    uint32_t NRxP = floor((mMillis - mPmmGps.getLastLocationTimeMs()) / 1000.0);
    if (NRxP > 999)
        NRxP = 999;

    mLiquidCrystal.printf("TxCt %06lu Sess %03hu", TxCt, mSimpleDataLogRx.getSourceSession());
    mLiquidCrystal.printf("MLCt %06lu H %05.fm",   MLCt, H);
    mLiquidCrystal.printf("Dist %05.fm Bear %03.f", Dist, Bear);
    mLiquidCrystal.printf("NTxP   %03us NRx %03us", NTxP, NRxP);
}



void RoutinePda::updatePdaData()    {
    mTxData.txCounter             = mSimpleDataLogRx.getVar_uint32_t(mTxIndexes.txCounter);
    mTxData.mainLoopCounter       = mSimpleDataLogRx.getVar_uint32_t(mTxIndexes.mainLoopCounter);
    mTxData.timeMillis            = mSimpleDataLogRx.getVar_uint32_t(mTxIndexes.timeMillis);
    mTxData.gpsLat                = mSimpleDataLogRx.getVar_int32_t(mTxIndexes.gpsLat);
    mTxData.gpsLon                = mSimpleDataLogRx.getVar_int32_t(mTxIndexes.gpsLon);
    mTxData.lastGpsLocationTimeMs = mSimpleDataLogRx.getVar_uint32_t(mTxIndexes.lastGpsLocationTimeMs);
    mTxData.barometerAltitude     = mSimpleDataLogRx.getVar_float(mTxIndexes.barometerAltitude);
}



int RoutinePda::updateTelemetryReception()  {
    if (mPmmTlm.updateReception())  {
        switch(mPmmTlm.getRxPacketAllInfoPtr()->port)   {
            case PORT_ID_SIMPLE_DATA_LOG:
                if (mSimpleDataLogRx.receivedPacket(mPmmTlm.getRxPacketAllInfoPtr()))
                    return PORT_ID_SIMPLE_DATA_LOG;
                break;
            default:;
        }
    }
    return 0;
}



// This MUST be exactly the same, for both transmitter and receiver.
void RoutinePda::addVarsSimpleDataLog()         {
    mSimpleDataLogRx.addBasicInfo        ();
    mSimpleDataLogRx.addAccelerometer    ();
    mSimpleDataLogRx.addGyroscope        ();
    mSimpleDataLogRx.addMpuTemperature   ();
    mSimpleDataLogRx.addBarometerPressure();
    mSimpleDataLogRx.addBarometerAltitude();
    mSimpleDataLogRx.addMagnetometer     ();
    mSimpleDataLogRx.addGpsLatLong       ();
    mSimpleDataLogRx.addGpsLastLocationTimeMs();
    mSimpleDataLogRx.addGpsAltitude      ();
    mSimpleDataLogRx.addGpsSatellites    ();

    // These are only for the receiver.
    mTxIndexes.txCounter             = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.transmissionCounter);
    mTxIndexes.mainLoopCounter       = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.mainLoopCounter);
    mTxIndexes.timeMillis            = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.timeMs);
    mTxIndexes.gpsLat                = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.gpsLatitude);
    mTxIndexes.gpsLon                = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.gpsLongitude);
    mTxIndexes.lastGpsLocationTimeMs = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.gpsLastLocationTimeMs);
    mTxIndexes.barometerAltitude     = mSimpleDataLogRx.getVarIndex(mSimpleDataLogRx.mStr.barometerAltitude);
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