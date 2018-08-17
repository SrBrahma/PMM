#ifndef PMM_PACKAGES_RECEPTION_h
#define PMM_PACKAGES_RECEPTION_h

#include "pmmPackageLog.h"
#include "pmmPackageString.h"

class PmmPackagesReception // Intended to have >1 Objects of this class, on the future! Maybe someday we will want to have one object for reception, and another for transmission!
{

private:

    PmmPackageLog *mPmmPackageLog;
    PmmPackageString *mPmmPackageString;

public:

    PmmPackagesReception();
    void receivedPacket();

}; // End of the class

#endif
