/* pmmPackageLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#if PMM_IS_PDA

#include <crc16.h>
#include <pmmPackageLog.h>
#include <pmmConsts.h>




// Receive Package Info Package
void PmmPackageLog::receivedPackageInfo(uint8_t* packetArray, uint8_t packetSize)
{
    uint16_t tempPackageCrc;
    /* struct receivedPackageInfoStruct
    {
        uint16_t entirePackageCrc;
        uint16_t receivedPacketsInBits; // Each bit corresponds to the successful packet received.
        uint8_t totalNumberPackets;
    } */

    if (packetSize < PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH) // If the packet size is smaller than the packet header length, it's invalid
        return;

    // First test the CRC, to see if the packet is valid.
    if ((packetArray[4] | (packetArray[5] << 8)) != (crc16(packetArray + 6, packetSize - 6), crc16(packetArray, 4)))
        return;

    tempPackageCrc = packetArray[6] | (packetArray[7] << 8);

    // if changed the CRC16 of the entire package, or is the first packet ever received

    if (tempPackageCrc != mReceivedPackageInfoStruct.entirePackageCrc || !mReceivedPackageInfoStruct.hasReceivedAnyPackageInfoBefore)
    {
        mReceivedPackageInfoStruct.hasReceivedAnyPackageInfoBefore = 1;
        mReceivedPackageInfoStruct.receivedPacketsInBits = 0;
        mReceivedPackageInfoStruct.entirePackageCrc      = tempPackageCrc;
        mReceivedPackageInfoStruct.totalNumberPackets    = (packetArray[8] & 0xF) + 1;
        // If is the first packet or if changed the entirePackageCrc, reset some parameters
    }

    mReceivedPackageInfoStruct.receivedPacketsInBits |= (1 << (packetArray[8] >> 4));
}


void PmmPackageLog::
#endif
