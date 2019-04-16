#include "pmmModules/messageLog/messageLog.h"
#include <crc.h>
#include <byteSelection.h>

int  PmmModuleMessageLog::sendString(uint8_t destinationAddress, telemetryQueuePriorities priority, uint8_t stringId)
{
    char tempString[MESSAGE_LOG_MAX_STRING_LENGTH_STORAGE];

    if (loadSelfString(tempString, stringId))
        return 1;

    sendString(destinationAddress, priority, stringId, tempString);

    return 0;
}

int  PmmModuleMessageLog::sendString(uint8_t destinationAddress, telemetryQueuePriorities priority, uint8_t stringId, char string[])
{
    if (!string)
        return 1;

    PacketToBeSent packetToBeSent;

    // 0!) The packet CRC will be added on the end of this function.
    // 1) Add the id of this string, and the total amount of strings.
    packetToBeSent.payload[PORT_MESSAGE_LOG_INDEX_STRING_X] = stringId;
    packetToBeSent.payload[PORT_MESSAGE_LOG_INDEX_OF_Y]     = mCurrentlNumberOfStrings;


    // 2) Now adds the payload, without the null-char.
    uint8_t stringLength = strlen(string);

    // 3) Copies the requested string to the buffer
    memcpy(packetToBeSent.payload + PORT_MESSAGE_LOG_HEADER_LENGTH, string, stringLength);
    // As the string was created with snprintf, and it always adds an '\0' at the end of the string (https://stackoverflow.com/a/50498477),
    // there is no need to care about '\0' here. (strncpy doesn't add a \0 if the source length > given length!, but snprintf(), does!)

    // 4) Lastly, add the CRC of this packet! We want to be certain (actually, it doesn't give us 100% of error detection! Something like
    //   1/65536 certainty, assuming random errors. If it happens, is it God giving us a message? We will probably never know!!)
    uint8_t  packetLength = PORT_MESSAGE_LOG_HEADER_LENGTH + stringLength;
    uint16_t crc16Var = crc16(packetToBeSent.payload + PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET + 1, packetLength - 2); // Remember to change this sum if you changed the Port Header!
    // Explaining:
    // arrayToCopy + PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET + 1
    //      The address is the next to the MSB CRC, so we sum (+) 1!
    // packetLength - 2
    //      The length to do the CRC is the total minus the 2 bytes used in the CRC itself!

    packetToBeSent.payload[PORT_MESSAGE_LOG_INDEX_LSB_CRC_PACKET] = LSB0(crc16Var);     // Little endian! First the Least Significant Byte!
    packetToBeSent.payload[PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET] = MSB0(crc16Var);     // Little endian! Then the Most Significant Byte!

    return packetLength;
}