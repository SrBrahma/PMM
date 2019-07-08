
#include <stdint.h> // for uint32_t
#include <crc.h>

#include "pmmConsts.h"
#include "pmmModules/messageLog/messageLog.h"
#include "pmmTelemetry/protocols.h"

void PmmModuleMessageLog::receivedPackageString(receivedPacketAllInfoStructType* packetInfo)
{
// 1) First check the length of the received payload.

    // 1.2) However, fo


    // 1) First check is the packet is valid
    // 1.a) If the packet size is smaller than the package header length, it's invalid
    if (packetInfo->payloadLength < PORT_MESSAGE_LOG_HEADER_LENGTH)
        return;

    // 1.b) Now test the CRC, to see if the packet content is valid
    if (((packetInfo->payload[PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET] << 8) | (packetInfo->payload[PORT_MESSAGE_LOG_INDEX_LSB_CRC_PACKET]))
                  != crc16(packetInfo->payload + PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET + 1, packetInfo->payloadLength - 2))
    // Explaining:
    // arrayToCopy + PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET + 1
    //      The address is the next to the MSB CRC, so we sum (+) 1!
    // packetLength - 2
    //      The length to do the CRC is the total minus the 2 bytes used in the CRC itself!
        return;

    // Now do stuff!
    //mStrings[PORT_STRING_MAX_STRINGS]
}
