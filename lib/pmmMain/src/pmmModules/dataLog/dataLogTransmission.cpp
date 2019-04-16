#include "crc.h"
#include "byteSelection.h"

#include "pmmModules/dataLog/dataLogGroupCore.h"
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/ports.h"           // For 

int PmmModuleDataLogGroupCore::sendDataLog(uint8_t destinationAddress, telemetryQueuePriorities priority)
{
    // --------------- DataLog Header 1.0 ---------------
    // [Positions] : [Function] : [ Length in Bytes ]
    //
    // a) [ 0 ] : [ CRC-8 of this header . ] : [ 1 ]
    // b) [ 1 ] : [ Session Identifier ... ] : [ 1 ]
    // c) [ 2 ] : [ DataLogInfo Identifier ] : [ 1 ]
    // d) [3,4] : [ CRC-16 of the Log .... ] : [ 2 ]
    // 
    //                    Total header length = 5 bytes.
    // --------------------------------------------------

    if (!mIsGroupLocked)
        buildLogInfoArray();

    if (!mPmmTelemetryPtr->availablePositionsInQueue(priority)) // Avoids building the packet uselessly
        return 1;

    PacketToBeSent packetToBeSent;

    uint8_t payloadLength;


    // 1) Add the Header.
    // 1.1) The CRC-8 is added after the other fields, on 4).
    // 1.2) Add the Session Identifier
    packetToBeSent.payload[PORT_DATA_LOG_INDEX_SESSION_ID]  = mSystemSession;
    // 1.3) Add the DataLogInfo Identifier
    packetToBeSent.payload[PORT_DATA_LOG_INDEX_DATA_LOG_ID] = mDataLogGroupId;
    // 1.4) The CRC-16 of the log is added after adding the Log data.

    payloadLength = PORT_DATA_LOG_HEADER_LENGTH;

    // 2) Add the Log data.
    for (unsigned actualVar = 0; actualVar < mNumberVariables; actualVar++)
    {
        memcpy(packetToBeSent.payload + payloadLength, mVariableAdrsArray[actualVar], mVariableSizeArray[actualVar]);
        payloadLength += mVariableSizeArray[actualVar];
    }

    // 3) Add the CRC-16 of the log to the Header (1.4)
    uint16_t crcValue = crc16(packetToBeSent.payload + PORT_DATA_LOG_HEADER_LENGTH, payloadLength - PORT_DATA_LOG_HEADER_LENGTH);
    packetToBeSent.payload[PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB] = LSB0(crcValue);
    packetToBeSent.payload[PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB] = LSB1(crcValue);

    // 4) Add the CRC-8 of the Header to the Header (1.1)
    packetToBeSent.payload[PORT_DATA_LOG_INDEX_CRC_8_HEADER] = crc8(packetToBeSent.payload + 1, PORT_DATA_LOG_HEADER_LENGTH - 1);

    // 5) Add the remaining fields and add it to the queue!
    packetToBeSent.addInfo(PMM_NEO_PROTOCOL_ID, PMM_TLM_ADDRESS_THIS_SYSTEM, destinationAddress, PORT_DATA_LOG_ID, payloadLength, priority);
    
    if (mPmmTelemetryPtr->addPacketToQueue(&packetToBeSent))
        return 2;

    mTransmissionCounter++;

    return 0;
}