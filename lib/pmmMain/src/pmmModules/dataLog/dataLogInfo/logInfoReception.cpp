/* PmmModuleDataLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "crc.h"
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/dataLog/dataLogInfo/logInfo.h"



// Received DataLogInfo Package
int PmmModuleDataLog::receivedDataLogInfo(receivedPacketAllInfoStructType* packetInfo)
{
// 1) If the packet size is smaller than the packet header length, it's invalid
    if (packetInfo->payloadLength < PORT_LOG_INFO_HEADER_LENGTH)
        return 1;

// 2) Test the CRC, to see if the packet is valid.
    if (((packetInfo->payload[PORT_LOG_INFO_INDEX_CRC_MSB] << 8) | (packetInfo->payload[PORT_LOG_INFO_INDEX_CRC_LSB]))
                                  != crc16(packetInfo->payload + PORT_LOG_INFO_INDEX_CRC_LSB + 2, packetInfo->payloadLength - 2))
        return 2;

// 3) Save the received packet on the memory
    saveReceivedDataLogInfo(&packetInfo->payload[PORT_LOG_INFO_INDEX_PAYLOAD_START],
                             packetInfo->payloadLength - PORT_LOG_INFO_HEADER_LENGTH,
                             packetInfo->sourceAddress,
                             packetInfo->payload[PORT_LOG_INFO_INDEX_SESSION_ID],
                             packetInfo->payload[PORT_LOG_INFO_INDEX_DATA_LOG_ID],
                             packetInfo->payload[PORT_LOG_INFO_INDEX_DATA_LOG_GROUP_LENGTH],
                             packetInfo->payload[PORT_LOG_INFO_INDEX_CURRENT_PACKET],
                             packetInfo->payload[PORT_LOG_INFO_INDEX_TOTAL_PACKETS]);

    return 0;
}


