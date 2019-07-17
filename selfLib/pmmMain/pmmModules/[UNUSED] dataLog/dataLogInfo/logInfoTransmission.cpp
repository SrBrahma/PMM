#include <string.h>

#include "crc.h"            // To calculate the CRC
#include "byteSelection.h"  // For unified LSBx functions
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/ports.h"



int PmmModuleDataLogGroupCore::sendDataLogInfo(uint8_t requestedPacket, uint8_t destinationAddress, telemetryQueuePriorities priority)
{
    if (!mIsGroupLocked) // So we won't be able to change the variables anymore in this LogData Identifier!
        buildLogInfoArray();

    if (requestedPacket >= mDataLogInfoPackets)
        return 1;

    if (!mPmmTelemetryPtr->availablePositionsInQueue(priority)) // Avoids building the packet uselessly
        return 2;

    PacketToBeSent packetToBeSent;

    uint8_t payloadLength = 0;

    // 1) Adds the DataLogInfo Header to the packet
    // { 1.1) The CRC-16 of the packet is added on the end of the function. }
    // 1.2) Add the Session Identifier
    packetToBeSent.payload[PORT_LOG_INFO_INDEX_SESSION_ID]     = mSystemSession;
    // 1.3) Add the Packet X
    packetToBeSent.payload[PORT_LOG_INFO_INDEX_CURRENT_PACKET] = requestedPacket;
    // 1.4) Add the Of Y Packets
    packetToBeSent.payload[PORT_LOG_INFO_INDEX_TOTAL_PACKETS]  = mDataLogInfoPackets;
    // 1.5) Add the DataLog Identifier
    packetToBeSent.payload[PORT_LOG_INFO_INDEX_DATA_LOG_ID]    = mDataLogGroupId;
    // 1.6) Add the DataLog Group Length
    packetToBeSent.payload[PORT_LOG_INFO_INDEX_DATA_LOG_GROUP_LENGTH] = mGroupLength;
    
    payloadLength = PORT_LOG_INFO_HEADER_LENGTH;


    // 2) Adds the DataLogInfo Payload, which was built on buildLogInfoArray().
    // 2.1) First, get the number of bytes this payload will have, as the last packet may not occupy all the available length.
    // This packet size is the total raw size minus the (current packet * packetPayloadLength).
    // If it is > maximum payload length, it will be equal to the payload length.
    uint16_t payloadBytesInThisPacket = mLogInfoContentArrayLength - (requestedPacket * PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
    if (payloadBytesInThisPacket > PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
        payloadBytesInThisPacket = PORT_LOG_INFO_MAX_PAYLOAD_LENGTH;

    // 2.2) Add the Payload.
    memcpy(packetToBeSent.payload + payloadLength, mLogInfoContentArray + (requestedPacket * PORT_LOG_INFO_MAX_PAYLOAD_LENGTH), payloadBytesInThisPacket);
    payloadLength += payloadBytesInThisPacket;

    // 3) CRC16 of this packet:
    uint16_t crc16ThisPacket = crc16(packetToBeSent.payload + 2, payloadLength - 2); // + 2 and - 2 skips the self CRC bytes.

    packetToBeSent.payload[PORT_LOG_INFO_INDEX_CRC_LSB] = LSB0(crc16ThisPacket); // Little endian!
    packetToBeSent.payload[PORT_LOG_INFO_INDEX_CRC_MSB] = LSB1(crc16ThisPacket);

    // 4) Add the remaining fields and add it to the queue!
    packetToBeSent.addInfo(PMM_NEO_PROTOCOL_ID, PMM_TLM_ADDRESS_THIS_SYSTEM, destinationAddress, PORT_DATA_LOG_INFO_ID, priority);

    mPmmTelemetryPtr->addPacketToQueue(&packetToBeSent);

    return 0;
}
