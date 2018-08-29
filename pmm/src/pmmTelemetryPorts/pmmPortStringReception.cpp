
#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#include <pmmTelemetryPorts/pmmPortString.h>
#include <crc16.h>

void PmmPortString::receivedPackageString(uint8_t* packetArray, uint8_t packetSize)
{
    // 1) First check is the packet is valid
    // 1.a) If the packet size is smaller than the package header length, it's invalid
    if (packetSize < PMM_PORT_STRING_HEADER_LENGTH)
        return;

    // 1.b) Now test the CRC, to see if the packet content is valid
    if (((packetArray[PMM_PORT_STRING_INDEX_MSB_CRC_PACKET] << 8) | (packetArray[PMM_PORT_STRING_INDEX_LSB_CRC_PACKET]))
            != crc16(packetArray + PMM_PORT_STRING_INDEX_MSB_CRC_PACKET + 1, packetSize - 2))
    // Explaining:
    // arrayToCopy + PMM_PORT_STRING_INDEX_MSB_CRC_PACKET + 1
    //      The address is the next to the MSB CRC, so we sum (+) 1!
    // packetLength - 2
    //      The length to do the CRC is the total minus the 2 bytes used in the CRC itself!
        return;

    // Now do stuff!
    //mStrings[PMM_PORT_STRING_MAX_STRINGS]
}
