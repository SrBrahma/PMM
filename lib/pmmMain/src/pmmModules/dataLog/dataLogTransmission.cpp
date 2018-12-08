#include "crc.h"
#include "byteSelection.h"

#include "pmmModules/dataLog/dataLog.h"


int PmmModuleDataLog::getDataLogInfoPacketToTransmit(uint8_t arrayToCopyTo[], uint8_t* packetLength)
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

    if (!arrayToCopyTo || !packetLength)
        return 1;

    if (!mIsLocked)
        updateLogInfoCombinedPayload();

// 1) Add the Header
    // 1.1) The CRC-8 is added after the other fields
    // 1.2) Add the Session Identifier
    arrayToCopyTo[PORT_DATA_LOG_INDEX_SESSION_ID]       = mSystemSession;
    // 1.3) Add the DataLogInfo Identifier
    arrayToCopyTo[PORT_DATA_LOG_INDEX_DATA_LOG_INFO_ID] = mDataLogInfoId;
    // 1.4) The CRC-16 of the log is added after adding the Log data.

    *packetLength = PORT_DATA_LOG_HEADER_LENGTH;

// 2) Add the Log data.
    for (unsigned actualVar = 0; actualVar < mNumberVariables; actualVar++)
    {
        memcpy(arrayToCopyTo + *packetLength, mVariableAdrsArray[actualVar], mVariableSizeArray[actualVar]);
        *packetLength += mVariableSizeArray[actualVar];
    }

// 3) Add the CRC-16 of the log to the Header (1.4)
    uint16_t crcValue = crc16(arrayToCopyTo + PORT_DATA_LOG_HEADER_LENGTH, *packetLength - PORT_DATA_LOG_HEADER_LENGTH);
    arrayToCopyTo[PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB] = LSB0(crcValue);
    arrayToCopyTo[PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB] = LSB1(crcValue);

// 4) Add the CRC-8 of the Header to the Header (1.1)
    arrayToCopyTo[PORT_DATA_LOG_INDEX_CRC_8_HEADER] = crc8(arrayToCopyTo, PORT_DATA_LOG_HEADER_LENGTH);

    return 0;
}