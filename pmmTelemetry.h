#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h

#include <Arduino.h>
#include <RH_RF95.h>
#include <pmmConsts.h>

#if PMM_USE_SD
    #include <SdFat.h>
#endif

#define PMM_TELEMETRY_PAYLOAD_LENGTH 255 // Must be 255. recv2() in RH_RF95.cpp.

class TelemetryManager
{
private:
    #if PMM_USE_SD
        SdFatSdioEX *mSdEx;
        File *mFile;
    #endif
    RH_RF95 rf95(PIN_RFM95_CS, PIN_RFM95_INT);
    uint8_t rfPayload[PMM_TELEMETRY_PAYLOAD_LENGTH];

public:
    TelemetryManager();
    int update();

};


#endif
