#include <string.h>

#include "crc.h"            // To calculate the CRC
#include "byteSelection.h"  // For unified LSBx functions
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/ports.h"



int PmmModuleDataLog::sendDataLogInfo(uint8_t requestedPacket, uint8_t destinationAddress, telemetryQueuePriorities priority)
{

    if (requestedPacket >= mDataLogInfoPackets)
        return 2;

    if (!mIsLocked) // So we won't be able to change the variables anymore in this LogData Identifier!
        updateLogInfoCombinedPayload();


    mPacketStruct.payloadLength = 0;

// 1) Adds the DataLogInfo Header to the packet
    // 1.1) The CRC-16 of the packet is added on the end of the function.
    // 1.2) Add the Session Identifier
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_SESSION_ID]     = mSystemSession;
    // 1.3) Add the Packet X
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_CURRENT_PACKET] = requestedPacket;
    // 1.4) Add the Of Y Packets
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_TOTAL_PACKETS]  = mDataLogInfoPackets;
    // 1.5) Add the DataLogInfo Identifier

// 2) Adds the DataLogInfo Payload, which was built on updateLogInfoCombinedPayload().
    // 2.1) First, get the number of bytes this payload will have, as the last packet may not occupy all the available length.
    // This packet size is the total raw size minus the (current packet * packetPayloadLength).
    // If it is > maximum payload length, it will be equal to the payload length.
    uint16_t payloadBytesInThisPacket = mLogInfoContentArrayLength - (requestedPacket * PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
    if (payloadBytesInThisPacket > PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
        payloadBytesInThisPacket = PORT_LOG_INFO_MAX_PAYLOAD_LENGTH;

    // 2.2) Add the Payload.
    memcpy(mPacketStruct.payload + mPacketStruct.payloadLength, mLogInfoContentArray, payloadBytesInThisPacket);
    mPacketStruct.payloadLength += payloadBytesInThisPacket;

// 3) CRC16 of this packet:
    uint16_t crc16ThisPacket = crc16(mPacketStruct.payload + 2, mPacketStruct.payloadLength - 2); // + 2 and - 2 skips the self CRC bytes.

    mPacketStruct.payload[PORT_LOG_INFO_INDEX_CRC_LSB] = LSB0(crc16ThisPacket); // Little endian!
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_CRC_MSB] = LSB1(crc16ThisPacket);

// 5) Add the remaining fields and add it to the queue!
    mPacketStruct.protocol           = PMM_NEO_PROTOCOL_ID;
    mPacketStruct.sourceAddress      = PMM_TELEMETRY_ADDRESS_THIS_SYSTEM;
    mPacketStruct.destinationAddress = destinationAddress;
    mPacketStruct.port               = PORT_DATA_LOG_ID;
    mPmmTelemetry->addPacketToQueue(&mPacketStruct, priority);

    return 0;
}
