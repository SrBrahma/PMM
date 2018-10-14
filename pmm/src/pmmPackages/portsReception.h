#ifndef PMM_PORTS_RECEPTION_h
#define PMM_PORTS_RECEPTION_h

#include "pmmPackages/dataLog/dataLog.h"
#include "pmmPackages/messageLog/pmmPortString.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h"

class PmmPortsReception
{

private:

    PmmPackageDataLog *mPmmPortLog;
    PmmPortString *mPmmPortString;

public:
    PmmPortsReception();
    int init(PmmPackageDataLog* PmmPackageDataLog, PmmPortString* PmmPortString);
    void receivedPacket(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);

}; // End of the class

#endif
