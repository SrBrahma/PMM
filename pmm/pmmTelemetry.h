/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h

#include <Arduino.h>
#include <RH_RF95.h>
#include <pmmConsts.h>
#include <pmmSd.h>

#define PMM_TELEMETRY_PAYLOAD_LENGTH 255 // Must be 255. recv2() in RH_RF95.cpp.

class PmmTelemetry
{
private:
    RH_RF95 mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT);
    uint8_t mRfPayload[PMM_TELEMETRY_PAYLOAD_LENGTH];
    PmmErrorsCentral *mPmmErrorsCentral;

public:
    PmmTelemetry();
    int init(PmmErrorsCentral *pmmErrorsCentral);
    int update();

};


#endif
