/* PmmPortLogReception.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "crc.h"
#include "pmmModules/dataLog/dataLog.h"

// Received Package Log Info Package
int PmmModuleDataLog::receivedDataLog(receivedPacketAllInfoStructType* packetInfo)
{
    // The packet structure can be checked at "extras/Telemetry Modules Guide.txt".
    
    #define groupLength   (packetInfo->payloadLength - PORT_DATA_LOG_HEADER_LENGTH)
    #define sourceSession (packetInfo->payload[PORT_DATA_LOG_INDEX_SESSION_ID])
    #define dataLogId     (packetInfo->payload[PORT_DATA_LOG_INDEX_DATA_LOG_ID])

    // 1) First, check the CRC-8 of this header, as there are important data on it.
    if (crc8(packetInfo->payload + 1, PORT_DATA_LOG_HEADER_LENGTH - 1) != packetInfo->payload[PORT_DATA_LOG_INDEX_CRC_8_HEADER])
    {
        return 1; // Ignore this packet
    }

    // 2) Save it!
    saveReceivedDataLog(&packetInfo->payload[PORT_DATA_LOG_PAYLOAD_START],
                         packetInfo->payloadLength - PORT_DATA_LOG_HEADER_LENGTH,
                         packetInfo->payload[PORT_DATA_LOG_INDEX_DATA_LOG_ID],
                         packetInfo->sourceAddress,
                         packetInfo->payload[PORT_DATA_LOG_INDEX_SESSION_ID]);

    #undef groupLength
    #undef sourceSession
    #undef dataLogId

    return 0;
}
