#include "crc.h"
#include "pmmPackages/dataLog/dataLog.h"
#include <string.h>


// 02/11/2018, Ok.
// LogInfo in Telemetry format, for transmission.
void PmmPackageDataLog::updateLogInfoCombinedPayload()
{
    // As stated in Telemetry Packages Guide, the LogInfo Package payload is
    // --------------------------------------------------
    //    [Positions] : [ Function ] : [ Length in Bytes ]
    //
    //    a) [ 6 ] : [ Number of variables ] : [ 1 ]
    //    b) [7,+] : [ Variable types .... ] : [ ceil(Number of variables/2) ]
    //    c) [+,+] : [ Variables Strings . ] : [ Depends on each variable name ]
    //    --------------------------------------------------
    //
    unsigned variableCounter;
    unsigned stringLength;              // The length withou null char!

    mLogInfoRawPayloadArrayLength = 0;  // Zero the length of the array.


// 1) Add the "Number of variables"
    mPackageLogInfoRawArray[0] = mLogNumberOfVariables;
    mLogInfoRawPayloadArrayLength ++;


// 2) Add the "Variable types"

    // 2.1) Adds the variables types in groups of two, as the types are always <= 4 bits, the group makes 8 bits.
    for (variableCounter = 0; variableCounter < mLogNumberOfVariables; variableCounter ++)
    {
        if (!variableCounter % 2) // If the counter is even (rest of division by 2 is 0)
            mPackageLogInfoRawArray[mLogInfoRawPayloadArrayLength] = (mVariableTypeArray[variableCounter] << 4);
        else // Else, the number is odd (rest of division by 2 is 1)
        {
            mPackageLogInfoRawArray[mLogInfoRawPayloadArrayLength] |= mVariableTypeArray[variableCounter];
            mLogInfoRawPayloadArrayLength++;
        }
    }

    // 2.2) If the the previous loop ended on a odd number (so the last conditional-valid value on the counter was even), increase the length.
    if (variableCounter % 2) 
        mLogInfoRawPayloadArrayLength++;


// 3) Add the Variable strings
    for (variableCounter = 0; variableCounter < mLogNumberOfVariables; variableCounter ++)
    {
        // As I couldn't find a way to use strnlen, made it!
        // Again, the stringLength doesn't include the '\0'.
        for (stringLength = 0;
             ((stringLength < (PMM_PACKAGE_DATA_LOG_MAX_STRING_LENGTH - 1)) && mVariableNameArray[variableCounter][stringLength]); // - 1 as the MAX_STRING_LENGTH includes the '\0'.
             stringLength++); 
            
        memcpy(mPackageLogInfoRawArray + mLogInfoRawPayloadArrayLength, mVariableNameArray[variableCounter], stringLength);
        mLogInfoRawPayloadArrayLength += stringLength;
        mPackageLogInfoRawArray[mLogInfoRawPayloadArrayLength] = '\0'; // Manually write the null terminating char, in case the string was broken.
        mLogInfoRawPayloadArrayLength ++;
    }
}



void PmmPackageDataLog::updateLogInfoInTelemetryFormat()
{
    uint16_t packetLength = 0; // The Package Header default length.
    uint16_t crc16ThisPacket;
    uint16_t payloadBytesInThisPacket;
    uint8_t packetCounter;

    // Calculate the total number of packets.
    mPackageLogInfoNumberOfPackets = ceil(mLogInfoRawPayloadArrayLength / (float) PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
    // This is different to PMM_PORT_LOG_INFO_MAX_PACKETS, as the macro is the maximum number of packets, and this variable is the current maximum
    // number of packets. This one varies with the current contents in LogInfo Package.


// 1) Copies the raw array content and the package header into the packets
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter++)
    {
        packetLength = PMM_PORT_LOG_INFO_HEADER_LENGTH; // The initial length is the default header length

        // This packet size is the total raw size minus the (current packet * packetPayloadLength).
        // If it is > maximum payload length, it will be equal to the payload length.
        payloadBytesInThisPacket = mLogInfoRawPayloadArrayLength - (packetCounter * PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
        if (payloadBytesInThisPacket > PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
            payloadBytesInThisPacket = PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH;

        packetLength += payloadBytesInThisPacket;

        // Adds the requested packet and the total number of packets - 1.
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_PACKET_X_OF_Y_MINUS_1] = (packetCounter << 4) | (mPackageLogInfoNumberOfPackets - 1);

        // Now adds the data, which was built on updatePackageLogInfoRaw(). + skips the packet header.
        memcpy(mPackageLogInfoTelemetryArray[packetCounter] + PMM_PORT_LOG_INFO_HEADER_LENGTH, mPackageLogInfoRawArray, payloadBytesInThisPacket);

        // Set the CRC16 of this packet fields as 0 (to calculate the entire packet CRC16 without caring about positions and changes in headers, etc)
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKET_LSB] = 0;
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKET_MSB] = 0;

        // Set the CRC16 of the entire package to 0.
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKAGE_LSB] = 0;
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKAGE_MSB] = 0;

        mLogInfoPackageCrc = crc16(mPackageLogInfoTelemetryArray[packetCounter], packetLength, mLogInfoPackageCrc); // The first crc16Package is = CRC16_DEFAULT_VALUE, as stated.
    }

// 2) Assign the entire package crc16 to all packets.
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter++)
    {
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKAGE_LSB] = mLogInfoPackageCrc;        // Little endian!
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKAGE_MSB] = mLogInfoPackageCrc >> 8;   //
    }

// 3) CRC16 of this packet:
    for (packetCounter = 0; packetCounter < mPackageLogInfoNumberOfPackets; packetCounter++)
    {
        crc16ThisPacket = crc16(mPackageLogInfoTelemetryArray[packetCounter], packetLength); // As the temporary CRC16 of this packet is know to be 0,
        //it can do the crc16 of the packet without skipping the crc16 fields

        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKET_LSB] = crc16ThisPacket;        // Little endian!
        mPackageLogInfoTelemetryArray[packetCounter][PMM_PORT_LOG_INFO_INDEX_CRC_PACKET_MSB] = crc16ThisPacket >> 8;   //

        mPackageLogInfoTelemetryArrayLengths[packetCounter] = packetLength;
    }
}
