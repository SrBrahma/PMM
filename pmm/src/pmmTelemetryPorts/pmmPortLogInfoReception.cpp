/* PmmPortLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmTelemetryPorts/pmmPortLog.h>
#include <pmmConsts.h>
#include <crc16.h>





// Received Package Log Info Package
void PmmPortLog::receivedPackageLogInfo(uint8_t payload[], pmmTelemetryPacketStatusStructType* packetStatus)
{
    uint16_t tempPackageCrc;
    unsigned packetId;

    // 1) If the packet size is smaller than the packet header length, it's invalid
    if (packetStatus->payloadLength < PMM_PORT_LOG_INFO_HEADER_LENGTH)
        return;

    // 2) Test the CRC, to see if the packet is valid.
    if (((payload[PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKET] << 8) | (payload[PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKET])) != crc16(payload + PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKET + 1, packetStatus->payloadLength - 2))
        return;

    tempPackageCrc = (payload[PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKAGE] << 8) | payload[PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKAGE];

    // 3) If changed the CRC16 of the entire package, or is the first packet ever received
    if (tempPackageCrc != mLogInfoPackageCrc || !mPackageLogInfoNumberOfPackets)
    {
        memset(mPackageLogInfoTelemetryArrayLengths, 0, PMM_PORT_LOG_INFO_MAX_PACKETS);
        mLogInfoPackageCrc = tempPackageCrc;
        mPackageLogInfoNumberOfPackets = (payload[PMM_PORT_LOG_INFO_INDEX_PACKET_X_OF_Y_MINUS_1] & 0xF) + 1; // Only get the 4 least significant bits, and sum 1!
        // If is the first packet or if changed the entirePackageCrc, reset some parameters
    }

    packetId = payload[PMM_PORT_LOG_INFO_INDEX_PACKET_X_OF_Y_MINUS_1] >> 4;

    // Copies the received array
    memcpy(mPackageLogInfoTelemetryArray[packetId], payload, packetStatus->payloadLength);

    mPackageLogInfoTelemetryArrayLengths[packetId] = packetStatus->payloadLength;

    for (packetId = 0; packetId < mPackageLogInfoNumberOfPackets; packetId ++)
    {
        if (mPackageLogInfoTelemetryArrayLengths == 0) // Test if any length is 0. If it is, a packet haven't been adquired yet.
            return; //  Leave the function. It has done it's job for now!
    }

    // If every packet has a length, all packets have been successfully been received.
    unitePackageInfoPackets(); // Packets of the world, unite!
}



void PmmPortLog::unitePackageInfoPackets()
{
    unsigned packetCounter;
    unsigned payloadLength;
    uint16_t logInfoRawArrayIndex = 0; // unsigned or uint32_t was giving me a "warning: comparison between signed and unsigned integer expressions [-Wsign-compare]". why?
    unsigned stringSizeWithNullChar;
    unsigned variableCounter;

    mPackageLogInfoRawArrayLength = 0;

    // 1) Copies all the packets into the big raw array
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter ++)
    {
        payloadLength = mPackageLogInfoTelemetryArrayLengths[packetCounter] - PMM_PORT_LOG_INFO_HEADER_LENGTH;
        // Copies the telemetry array to the raw array. Skips the headers in the telemetry packet.
        memcpy(mPackageLogInfoRawArray + mPackageLogInfoRawArrayLength,
               mPackageLogInfoTelemetryArray[packetCounter] + PMM_PORT_LOG_INFO_HEADER_LENGTH,
               payloadLength);

        // Increases the raw array length by the copied telemetry array length.
        mPackageLogInfoRawArrayLength += payloadLength;
    }



    // 2) Now extracts all the info from the raw array.

    // 2.1) First get the number of variables
    mLogNumberOfVariables = mPackageLogInfoRawArray[0];
    // 2.2) Get the variable types and sizes
    for (variableCounter = 0; variableCounter < mLogNumberOfVariables; variableCounter++)
    {
        if (variableCounter % 2) // If is odd (if rest is 1)
        {
            mVariableTypeArray[variableCounter] = mPackageLogInfoRawArray[logInfoRawArrayIndex] & 0xF; // Get the 4 Least significant bits
        }
        else // Is even (if rest is 0). This will happen first
        {
            logInfoRawArrayIndex++;
            mVariableTypeArray[variableCounter] = mPackageLogInfoRawArray[logInfoRawArrayIndex] >> 4; // Get the 4 Most significant bits
        }
        mVariableSizeArray[variableCounter] = variableTypeToVariableSize(mVariableTypeArray[variableCounter]);
    }

    // 2.3) Now get the strings of the variables
    logInfoRawArrayIndex++;
    for (variableCounter = 0; logInfoRawArrayIndex < mPackageLogInfoRawArrayLength - 1; variableCounter++)
    {
        stringSizeWithNullChar = strlen((char*)&mPackageLogInfoRawArray[logInfoRawArrayIndex]) + 1; // Includes '\0'.
        memcpy(mVariableNameArray[variableCounter], &mPackageLogInfoRawArray[logInfoRawArrayIndex], stringSizeWithNullChar);
        logInfoRawArrayIndex += stringSizeWithNullChar;
    }

    // Finished!
}
