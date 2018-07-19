/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h


#include <Arduino.h>
#include <RH_RF95.h>
#include <pmmConsts.h>
#include <pmmSd.h>

class PmmTelemetry
{
    //PmmTelemetry(); // https://stackoverflow.com/a/12927220

private:
    uint8_t mRfPayload[PMM_TELEMETRY_MAX_PAYLOAD_LENGTH];
    PmmErrorsCentral *mPmmErrorsCentral;
    RH_RF95 mRf95;
    uint32_t mNextMillisPackageLog;

public:
    PmmTelemetry();
    int init(PmmErrorsCentral *pmmErrorsCentral);
    int update();
    int updateReception();
    int updateTransmission();

};


#endif
