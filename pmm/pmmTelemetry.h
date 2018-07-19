/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h

#include <pmmConsts.h>
#include <Arduino.h>
#include <RH_RF95.h>

#include <pmmSd.h>

#define PMM_TELEMETRY_PAYLOAD_LENGTH 255 // Must be 255. recv2() in RH_RF95.cpp.

class PmmTelemetry
{
    PmmTelemetry(); // https://stackoverflow.com/a/12927220

private:
    uint8_t mRfPayload[PMM_TELEMETRY_PAYLOAD_LENGTH];
    PmmErrorsCentral *mPmmErrorsCentral;
    RH_RF95 mRf95;
    
public:
    int init(PmmErrorsCentral *pmmErrorsCentral);
    int update();
    int updateReception();
    int updateTransmission();

};


#endif
