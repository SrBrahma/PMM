/* pmmPackageLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */


#include <pmmPackages/pmmPackagesReception.h>
#include <pmmPackages/pmmPackageLog.h>
#include <pmmPackages/pmmPackageString.h>

PmmPackagesReception::PmmPackagesReception()
{
}

int PmmPackagesReception::init(PmmPackageLog* pmmPackageLog, PmmPackageString* pmmPackageString)
{
    mPmmPackageLog = pmmPackageLog;
    mPmmPackageString = pmmPackageString;
    return 0;
}

void PmmPackagesReception::receivedPacket(uint8_t packetArray[], uint16_t packetLength)
{
    // 1) Which kind of packet is it?

    // The PMM will only deal with these packages types if it is a PDA. (may be changed in the future)
    #if PMM_IS_PDA
        if (!memcmp(packetArray, PMM_TELEMETRY_HEADER_TYPE_LOG, 4)) // MLOG
            mPmmPackageLog->receivedPackageLog(packetArray, packetLength);
            return;
        if (!memcmp(packetArray, PMM_TELEMETRY_HEADER_TYPE_LOG_INFO, 4)) // MLIN
            mPmmPackageLog->receivedPackageLogInfo(packetArray, packetLength);
            return;
        if (!memcmp(packetArray, PMM_TELEMETRY_HEADER_TYPE_STRING , 4)) // MSTR
            mPmmPackageString->receivedPackageString(packetArray, packetLength);
            return;
    #endif


    if (!memcmp(packetArray, PMM_TELEMETRY_HEADER_TYPE_REQUEST, 4)) // MRQT
        return;



    // 2.b) If the packet type is unknown / invalid

}
