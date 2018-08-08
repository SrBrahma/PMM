
#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#include <pmmPackageString.h>
#include <crc16.h>

void PmmPackageString::receivedPackageString(uint8_t* packetArray, uint8_t packetSize)
{
    // For now the code is a little dumb. But works.

    // 1) First check is the packet is valid
    // 1.a) If the packet size is smaller than the package header length, it's invalid
    if (packetSize < PMM_TELEMETRY_PACKAGE_STRING_HEADER_LENGTH)
        return;

    // First test the CRC, to see if the packet is valid.
    if ((packetArray[5] << 8) | (packetArray[4]) != (crc16(packetArray + 6, packetSize - 6), crc16(packetArray, PMM_TELEMETRY_HEADER_TYPE_LENGTH)))
        return;

    mStrings[PMM_PACKAGE_STRING_MAX_STRINGS]
}
