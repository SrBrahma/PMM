#ifndef PMM_PACKAGES_RECEPTION_h
#define PMM_PACKAGES_RECEPTION_h

#include <pmmPackages/pmmPackageLog.h>
#include <pmmPackages/pmmPackageString.h>

class PmmPackagesReception // Intended to have >1 Objects of this class, on the future! Maybe someday we will want to have one object for reception, and another for transmission!
{

private:

    PmmPackageLog *mPmmPackageLog;
    PmmPackageString *mPmmPackageString;

public:
    PmmPackagesReception();
    int init(PmmPackageLog* pmmPackageLog, PmmPackageString* pmmPackageString);
    void receivedPacket(uint8_t packetArray[], uint16_t packetLength);

}; // End of the class

#endif
