#include "crc.h"
#include "pmmPackages/dataLog/dataLog.h"

// Log Info Package in Telemetry format (MLIN)
void PmmPackageDataLog::updatePackageLogInfoRaw()
{
    uint8_t variableCounter;
    uint8_t stringLengthWithNullChar;

    mPackageLogInfoRawArray[0] = mLogNumberOfVariables;
    mPackageLogInfoRawArrayLength = 1;

    // 1) Add the variable types
    for (variableCounter = 0; variableCounter < mLogNumberOfVariables; variableCounter++)
    {
        // 1.1) If is odd (if rest is 1)
        if (variableCounter % 2)
        {
            mPackageLogInfoRawArray[mPackageLogInfoRawArrayLength] |= mVariableTypeArray[variableCounter]; // Add it on the right
            mPackageLogInfoRawArrayLength++;
        }

        // 1.2) Is even (rest is 0). As it happens first than the odd option, no logical OR is needed.
        else
            mPackageLogInfoRawArray[mPackageLogInfoRawArrayLength] = mVariableTypeArray[variableCounter] << 4; // Shift Left 4 positions to add to the left
    }

    // 1.3) If for loop ended on a even number (and now the variable is odd due to the final increment that made it >= mLogNumberOfVariables)
    if (variableCounter % 2)
        mPackageLogInfoRawArrayLength++; // As this variable only increased in odd numbers.


    // 2) Now add the length of the following strings, and the strings of each variable
    for (variableCounter = 0; variableCounter < mLogNumberOfVariables; variableCounter++)
    {

        // 2.1) Adds the strings, with the null-char
        stringLengthWithNullChar = strlen(mVariableNameArray[variableCounter]) + 1; // Doesn't count the '\0'.
        memcpy(&mPackageLogInfoRawArray[mPackageLogInfoRawArrayLength], mVariableNameArray[variableCounter], stringLengthWithNullChar);
        mPackageLogInfoRawArrayLength += stringLengthWithNullChar;
    }

    // 3) Calculate the total number of packets.
    mPackageLogInfoNumberOfPackets = ceil(mPackageLogInfoRawArrayLength / (float) PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
    // This is different to PMM_PORT_LOG_INFO_MAX_PACKETS, as the macro is the maximum number of packets, and this variable is the current maximum
    // number of packets. This one varies with the current contents in MLIN Package.
}



void PmmPackageDataLog::updatePackageLogInfoInTelemetryFormat()
{
    // The port format is in dataLog.h

    uint16_t packetLength = 0; // The Package Header default length.
    uint16_t crc16ThisPacket;
    mLogInfoPackageCrc;
    uint16_t payloadBytesInThisPacket;
    uint8_t packetCounter;


    // 1) Copies the raw array content and the package header into the packets
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter++)
    {
        packetLength = PMM_PORT_LOG_INFO_HEADER_LENGTH; // The initial length is the default header length

        // This packet size is the total raw size minus the (current packet * packetPayloadLength).
        // If it is > maximum payload length, it will be equal to the payload length.
        payloadBytesInThisPacket = mPackageLogInfoRawArrayLength - (packetCounter * PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
        if (payloadBytesInThisPacket > PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
            payloadBytesInThisPacket = PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH;

        packetLength += payloadBytesInThisPacket;

        // Adds the requested packet and the total number of packets - 1.
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_PACKET_X_OF_Y_MINUS_1] = (packetCounter << 4) | (mPackageLogInfoNumberOfPackets - 1);

        // Now adds the data, which was built on updatePackageLogInfoRaw(). + skips the packet header.
        memcpy(mPackageLogInfoTelemetryArray[packetCounter] + PMM_PORT_LOG_INFO_HEADER_LENGTH, mPackageLogInfoRawArray, payloadBytesInThisPacket);

        // Set the CRC16 of this packet fields as 0 (to calculate the entire packet CRC16 without caring about positions and changes in headers, etc)
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKET] = 0;
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKET] = 0;

        // Set the CRC16 of the entire package to 0.
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKAGE] = 0;
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKAGE] = 0;

        mLogInfoPackageCrc = crc16(mPackageLogInfoTelemetryArray[packetCounter], packetLength, mLogInfoPackageCrc); // The first crc16Package is = CRC16_DEFAULT_VALUE, as stated.
    }

    // 2) Assign the entire package crc16 to all packets.
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter++)
    {
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKAGE] = mLogInfoPackageCrc;        // Little endian!
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKAGE] = mLogInfoPackageCrc >> 8;   //
    }

    // 3) CRC16 of this packet:
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter++)
    {
        crc16ThisPacket = crc16(mPackageLogInfoTelemetryArray[packetCounter], packetLength); // As the temporary CRC16 of this packet is know to be 0,
        //it can do the crc16 of the packet without skipping the crc16 fields

        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKET] = crc16ThisPacket;        // Little endian!
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKET] = crc16ThisPacket >> 8;   //

        mPackageLogInfoTelemetryArrayLengths[packetCounter] = packetLength;
    }
}
