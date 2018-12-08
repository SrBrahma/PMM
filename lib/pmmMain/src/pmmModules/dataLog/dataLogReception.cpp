/* PmmPortLogReception.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "crc.h"
#include "pmmModules/dataLog/dataLog.h"

// Received Package Log Info Package
int PmmModuleDataLog::receivedDataLog(receivedPacketAllInfoStructType* packetInfo)
{
    // The packet structure can be seen at "extras/Telemetry Modules Guide.txt".

    // 1) First, check the CRC-8 of this header, as there are important data on it.
    if (crc8(packetInfo->payload + 1, PORT_DATA_LOG_HEADER_LENGTH - 1) != packetInfo->payload[PORT_DATA_LOG_INDEX_CRC_8_HEADER])
    {
        return 1; // Ignore this packet
    }

    return 0;
}
