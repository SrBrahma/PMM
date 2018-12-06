/* PmmPortLogReception.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "crc.h"
#include "pmmModules/dataLog/dataLog.h"

// Received Package Log Info Package
void PmmModuleDataLog::receivedDataLog(receivedPacketAllInfoStructType* packetInfo)
{
    //        --------------- DataLog Header 1.0 ---------------
    //        [Positions] : [Function] : [ Length in Bytes ]
    //
    //        a) [ 0 ] : [ CRC 8 of this header . ] : [ 1 ]
    //        b) [ 1 ] : [ Session Identifier ... ] : [ 1 ]
    //        c) [2,3] : [ DataLogInfo related CRC 16 ] : [ 2 ]
    //        d) [4,5] : [ CRC 16 of the Log .... ] : [ 2 ]
    //
    //                            Total header length = 6 bytes.
    //        --------------------------------------------------

    //        -------------- DataLog Payload 1.0 ---------------
    //        [Positions] : [ Function ] : [ Length in Bytes ]
    //
    //        a) [6,+] : [ Data Log ] : [ LogLength ]
    //        --------------------------------------------------

    // 1) First, check the CRC 8 of this header, as there are important data in the header.
    if (crc8(packetInfo->payload + 1, PORT_DATA_LOG_HEADER_LENGTH - 1) != packetInfo->payload[PORT_DATA_LOG_INDEX_CRC_8_HEADER])
    {
        return; // For now the package will just be ignored. On the future it can be more treated.
    }
    if (packetInfo->payload[packetInfo->payloadLength])
        return; // dumb
}
