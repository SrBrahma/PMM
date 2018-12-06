/* PmmModuleDataLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "crc.h"
#include "pmmModules/dataLog/dataLog.h"



// Received Package Log Info Package
void PmmModuleDataLog::receivedLogInfo(receivedPacketAllInfoStructType* packetInfo)
{

    unsigned packetId;

// 1) If the packet size is smaller than the packet header length, it's invalid
    if (packetInfo->payloadLength < PORT_LOG_INFO_HEADER_LENGTH)
        return;


// 2) Test the CRC, to see if the packet is valid.
    if (((packetInfo->payload[PORT_LOG_INFO_INDEX_CRC_PACKET_MSB] << 8) | (packetInfo->payload[PORT_LOG_INFO_INDEX_CRC_PACKET_LSB]))
                              != crc16(packetInfo->payload + PORT_LOG_INFO_INDEX_CRC_PACKET_MSB + 1, packetInfo->payloadLength - 2))
        return;



// 4) Get the packetId from the received packet
    packetId = packetInfo->payload[PORT_LOG_INFO_INDEX_PACKET_X];

    // Copies the received array
    memcpy(mDataLogInfoTelemetryArray[packetId], packetInfo->payload, packetInfo->payloadLength);

    mDataLogInfoTelemetryArrayLengths[packetId] = packetInfo->payloadLength;

    for (packetId = 0; packetId < mDataLogInfoPackets; packetId ++)
    {
        if (mDataLogInfoTelemetryArrayLengths == 0) // Test if any length is 0. If it is, a packet haven't been adquired yet.
            return; //  Leave the function. It has done it's job for now!
    }

    // If every packet has a length, all packets have been successfully been received.
    unitePackageInfoPackets(); // Packets of the world, unite!
}



void PmmModuleDataLog::unitePackageInfoPackets()
{
    unsigned packetCounter;
    unsigned payloadLength;
    uint16_t logInfoRawArrayIndex = 0; // unsigned or uint32_t was giving me a "warning: comparison between signed and unsigned integer expressions [-Wsign-compare]". why?
    unsigned stringSizeWithNullChar;
    unsigned variableCounter;

    mLogInfoRawPayloadArrayLength = 0;

// 1) Copies all the packets into the big raw array
    for (packetCounter = 0; packetCounter < mDataLogInfoPackets; packetCounter ++)
    {
        payloadLength = mDataLogInfoTelemetryArrayLengths[packetCounter] - PORT_LOG_INFO_HEADER_LENGTH;
        // Copies the telemetry array to the raw array. Skips the headers in the telemetry packet.
        memcpy(mDataLogInfoTelemetryRawArray + mLogInfoRawPayloadArrayLength,
               mDataLogInfoTelemetryArray[packetCounter] + PORT_LOG_INFO_HEADER_LENGTH,
               payloadLength);

        // Increases the raw array length by the copied telemetry array length.
        mLogInfoRawPayloadArrayLength += payloadLength;
    }



// 2) Now extracts all the info from the raw array.

    // 2.1) First get the number of variables
    mLogNumberOfVariables = mDataLogInfoTelemetryRawArray[0];
    
    // 2.2) Get the variable types and sizes
    for (variableCounter = 0; variableCounter < mLogNumberOfVariables; variableCounter++)
    {
        if (variableCounter % 2) // If is odd (if rest is 1)
        {
            mVariableTypeArray[variableCounter] = mDataLogInfoTelemetryRawArray[logInfoRawArrayIndex] & 0xF; // Get the 4 Least significant bits
        }
        else // Is even (if rest is 0). This will happen first
        {
            logInfoRawArrayIndex++;
            mVariableTypeArray[variableCounter] = mDataLogInfoTelemetryRawArray[logInfoRawArrayIndex] >> 4; // Get the 4 Most significant bits
        }
        mVariableSizeArray[variableCounter] = variableTypeToVariableSize(mVariableTypeArray[variableCounter]);
    }

    // 2.3) Now get the strings of the variables
    logInfoRawArrayIndex++;
    for (variableCounter = 0; logInfoRawArrayIndex < mLogInfoRawPayloadArrayLength - 1; variableCounter++)
    {
        stringSizeWithNullChar = strlen((char*)&mDataLogInfoTelemetryRawArray[logInfoRawArrayIndex]) + 1; // Includes '\0'.
        memcpy(mVariableNameArray[variableCounter], &mDataLogInfoTelemetryRawArray[logInfoRawArrayIndex], stringSizeWithNullChar);
        logInfoRawArrayIndex += stringSizeWithNullChar;
    }

    // Finished!
}
