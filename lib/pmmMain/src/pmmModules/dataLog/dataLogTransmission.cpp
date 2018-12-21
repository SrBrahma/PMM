#include "crc.h"
#include "byteSelection.h"

#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/ports.h"           // For 

int PmmModuleDataLog::sendDataLog()
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

// We will only add to the telemetry queue this packet if there are no packets to be sent ahead of this one! We prefer
    // to send updated logs! Not old ones!
    // And I might change it on the future!
    // However, packets from other modules added to the queue with a higher priority still may be added, and will be sent first.
    // On the future, as always, I may make it better, maybe replacing the old packet on the queue with a new one.
    if (mPmmTelemetry->getTotalPacketsRemainingOnQueue() > 0)
        return 1;

    if (!mIsLocked)
        updateLogInfoCombinedPayload();

// 1) Add the Header
    // 1.1) The CRC-8 is added after the other fields
    // 1.2) Add the Session Identifier
    mPacketStruct.payload[PORT_DATA_LOG_INDEX_SESSION_ID]       = mSystemSession;
    // 1.3) Add the DataLogInfo Identifier
    mPacketStruct.payload[PORT_DATA_LOG_INDEX_DATA_LOG_ID] = mDataLogId;
    // 1.4) The CRC-16 of the log is added after adding the Log data.

    mPacketStruct.payloadLength = PORT_DATA_LOG_HEADER_LENGTH;

// 2) Add the Log data.
    for (unsigned actualVar = 0; actualVar < mNumberVariables; actualVar++)
    {
        memcpy(mPacketStruct.payload + mPacketStruct.payloadLength, mVariableAdrsArray[actualVar], mVariableSizeArray[actualVar]);
        mPacketStruct.payloadLength += mVariableSizeArray[actualVar];
    }

// 3) Add the CRC-16 of the log to the Header (1.4)
    uint16_t crcValue = crc16(mPacketStruct.payload + PORT_DATA_LOG_HEADER_LENGTH, mPacketStruct.payloadLength - PORT_DATA_LOG_HEADER_LENGTH);
    mPacketStruct.payload[PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB] = LSB0(crcValue);
    mPacketStruct.payload[PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB] = LSB1(crcValue);

// 4) Add the CRC-8 of the Header to the Header (1.1)
    mPacketStruct.payload[PORT_DATA_LOG_INDEX_CRC_8_HEADER] = crc8(mPacketStruct.payload, PORT_DATA_LOG_HEADER_LENGTH);

// 5) Add the remaining fields and add it to the queue!
    mPacketStruct.protocol           = PMM_NEO_PROTOCOL_ID;
    mPacketStruct.sourceAddress      = PMM_TELEMETRY_ADDRESS_THIS_SYSTEM;
    mPacketStruct.destinationAddress = PMM_TELEMETRY_ADDRESS_BROADCAST;
    mPacketStruct.port               = PORT_DATA_LOG_ID;
    mPmmTelemetry->addPacketToQueue(&mPacketStruct, PMM_TELEMETRY_QUEUE_PRIORITY_LOW);
    return 0;
}