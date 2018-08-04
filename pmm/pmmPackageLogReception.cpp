/* pmmPackageLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#if PMM_IS_PDA

#include <crc16.h>
#include <pmmPackageLog.h>
#include <pmmConsts.h>



// Receive Package Info Package
void PmmPackageLog::receivedPackageInfo(uint8_t* packetArray, uint8_t packetSize)
{
    uint16_t tempPackageCrc;
    unsigned packetId;

    // If the packet size is smaller than the packet header length, it's invalid
    if (packetSize < PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH)
        return;

    // First test the CRC, to see if the packet is valid.
    if ((packetArray[4] | (packetArray[5] << 8)) != (crc16(packetArray + 6, packetSize - 6), crc16(packetArray, 4)))
        return;

    tempPackageCrc = packetArray[6] | (packetArray[7] << 8);

    // if changed the CRC16 of the entire package, or is the first packet ever received
    if (tempPackageCrc != mReceivedPackageInfoStruct.entirePackageCrc || !mPackageLogInfoNumberOfPackets)
    {
        memset(mPackageLogInfoTelemetryArrayLengths, 0, PMM_TELEMETRY_PACKAGE_LOG_INFO_MAX_PACKETS);
        mLogInfoPackageCrc = tempPackageCrc;
        mPackageLogInfoNumberOfPackets = (packetArray[8] & 0xF) + 1;
        // If is the first packet or if changed the entirePackageCrc, reset some parameters
    }

    packetId = packetArray[8] >> 4;

    // Copies the received array
    memcpy(mPackageLogInfoTelemetryArray[packetId], packetArray, packetSize);

    mPackageLogInfoTelemetryArrayLengths[packetId] = packetSize;

    for (packetId = 0; packetId < mPackageLogInfoNumberOfPackets; packetId ++)
    {
        if (mPackageLogInfoTelemetryArrayLengths == 0) // Test if any length is 0. If it is, a packet haven't been adquired yet.
            return; //  Leave the function. It has done it's job for now!
    }

    // If every packet has a length, all packets have been successfully been received.
    unitePackageInfoPackets(); // Packets of the world, unite!
}


void PmmPackageLog::unitePackageInfoPackets()
{
    unsigned packetCounter;
    unsigned payloadLength;
    unsigned logInfoRawArrayIndex = 0;
    unsigned stringSize;

    mPackageLogInfoRawArrayLength = 0;

    // Copies all the packets into the big raw array
    for (packetCounter = 0; packetCounter < mReceivedPackageInfoStruct.totalNumberPackets; packetCounter ++)
    {
        payloadLength = mPackageLogInfoTelemetryArrayLengths[packetCounter] - PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH;
        // Copies the telemetry array to the raw array. Skips the headers in the telemetry packet.
        memcpy(mPackageLogInfoRawArray + mPackageLogInfoRawArrayLength,
               mPackageLogInfoTelemetryArray[packetCounter] + PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH,
               payloadLength);

        // Increases the raw array length by the copied telemetry array length.
        mPackageLogInfoRawArrayLength += payloadLength;
    }



    // Now extracts all the info from the raw array.

    mLogNumberOfVariables = mPackageLogInfoRawArray[0]; // First get the number of variables

    // Get the variable types and sizes
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

    // Now get the strings of the variables
    logInfoRawArrayIndex++;
    for (variableCounter = 0; logInfoRawArrayIndex < mPackageLogInfoRawArrayLength - 1; variableCounter++)
    {
        stringSize = strlen(mPackageLogInfoRawArray[logInfoRawArrayIndex]) + 1; // Includes '\0'.
        memcpy(mVariableNameArray[variableCounter], mPackageLogInfoRawArray[logInfoRawArrayIndex], stringSize);
        logInfoRawArrayIndex += stringSize;
    }

    // Finished!
}
#endif
