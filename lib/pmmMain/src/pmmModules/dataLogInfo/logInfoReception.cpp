/* PmmModuleDataLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "crc.h"
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/dataLogInfo/logInfo.h"



// Received DataLogInfo Package
int PmmModuleDataLog::receivedLogInfo(receivedPacketAllInfoStructType* packetInfo)
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
                             packetInfo->payload[PORT_LOG_INFO_INDEX_CURRENT_PACKET],
                             packetInfo->payload[PORT_LOG_INFO_INDEX_TOTAL_PACKETS],
                             packetInfo->payload[PORT_LOG_INFO_INDEX_DATA_LOG_ID],
                             packetInfo->sourceAddress,
                             packetInfo->payload[PORT_LOG_INFO_INDEX_SESSION_ID]);

    // unitePackageInfoPackets(); 

    return 0;
}


// Packets of the world, unite!
/*
void PmmModuleDataLog::unitePackageInfoPackets()
{
    unsigned packetCounter;
    unsigned payloadLength;
    uint16_t logInfoRawArrayIndex = 0; // unsigned or uint32_t was giving me a "warning: comparison between signed and unsigned integer expressions [-Wsign-compare]". why?
    unsigned stringSizeWithNullChar;
    unsigned variableCounter;

    mLogInfoContentArrayLength = 0;

// 1) Copies all the packets into the big raw array
    for (packetCounter = 0; packetCounter < mDataLogInfoPackets; packetCounter ++)
    {
        //payloadLength = 
        // Copies the telemetry array to the raw array. Skips the headers in the telemetry packet.
        memcpy(mLogInfoContentArray + mLogInfoContentArrayLength,
               mDataLogInfoTelemetryArray[packetCounter] + PORT_LOG_INFO_HEADER_LENGTH,
               payloadLength);

        // Increases the raw array length by the copied telemetry array length.
        mLogInfoContentArrayLength += payloadLength;
    }



// 2) Now extracts all the info from the raw array.

    // 2.1) First get the number of variables
    mNumberVariables = mLogInfoContentArray[0];
    
    // 2.2) Get the variable types and sizes
    for (variableCounter = 0; variableCounter < mNumberVariables; variableCounter++)
    {
        if (variableCounter % 2) // If is odd (if rest is 1)
        {
            mVariableTypeArray[variableCounter] = mLogInfoContentArray[logInfoRawArrayIndex] & 0xF; // Get the 4 Least significant bits
        }
        else // Is even (if rest is 0). This will happen first
        {
            logInfoRawArrayIndex++;
            mVariableTypeArray[variableCounter] = mLogInfoContentArray[logInfoRawArrayIndex] >> 4; // Get the 4 Most significant bits
        }
        mVariableSizeArray[variableCounter] = variableTypeToVariableSize(mVariableTypeArray[variableCounter]);
    }

    // 2.3) Now get the strings of the variables
    logInfoRawArrayIndex++;
    for (variableCounter = 0; logInfoRawArrayIndex < mLogInfoContentArrayLength - 1; variableCounter++)
    {
        stringSizeWithNullChar = strlen((char*)&mLogInfoContentArray[logInfoRawArrayIndex]) + 1; // Includes '\0'.
        memcpy(mVariableNameArray[variableCounter], &mLogInfoContentArray[logInfoRawArrayIndex], stringSizeWithNullChar);
        logInfoRawArrayIndex += stringSizeWithNullChar;
    }

    // Finished!
}
*/