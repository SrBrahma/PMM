#ifndef PMM_PORTS_RECEPTION_h
#define PMM_PORTS_RECEPTION_h

#include "pmmPackages/dataLog/dataLog.h"
#include "pmmPackages/messageLog/messageLog.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h"

class PmmPortsReception
{

private:

    PmmPackageDataLog *mPmmPackageDataLog;
    PmmPackageMessageLog *mPmmPackageMessageLog;

public:
    PmmPortsReception();
    int init(PmmPackageDataLog* PmmPackageDataLog, PmmPackageMessageLog* PmmPackageMessageLog);
    void receivedPacket(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);

}; // End of the class

#endif
