#ifndef PMM_PACKAGES_RECEPTION_h
#define PMM_PACKAGES_RECEPTION_h

#include <pmmTelemetryPorts/pmmPortLog.h>
#include <pmmTelemetryPorts/pmmPortString.h>

class PmmPortsReception // Intended to have >1 Objects of this class, on the future! Maybe someday we will want to have one object for reception, and another for transmission!
{

private:

    PmmPortLog *mPmmPortLog;
    PmmPortString *mPmmPortString;

public:
    PmmPortsReception();
    int init(PmmPortLog* PmmPortLog, PmmPortString* PmmPortString);
    void receivedPacket(uint8_t packetArray[], uint16_t packetLength);

}; // End of the class

#endif
