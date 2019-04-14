#include "pmmModules/messageLog/messageLog.h"

uint8_t PmmModuleMessageLog::sendString(uint8_t* arrayToCopy, uint8_t requestedStringId)
{
    char tempString[MESSAGE_LOG_MAX_STRING_LENGTH_STORAGE];
    // Port format is in pmmPortString.h

    // If the requested ID is invalid.
    if (requestedStringId >= mActualNumberOfStrings)
        return 0;

    // 0!) The packet CRC will be added on the end of this function.

    // 1) Add the id of this string, and the total amount of strings - 1
    arrayToCopy[PORT_MESSAGE_LOG_INDEX_STRING_X] = requestedStringId;
    arrayToCopy[PORT_MESSAGE_LOG_INDEX_OF_Y_MINUS_1] = mActualNumberOfStrings - 1;

    if (loadStringFromSd(tempString, requestedStringId))
        return 1;

    // 2) Now adds the payload, the null-terminated string!
    uint8_t stringLengthWithNullChar = strlen(tempString) + 1;

    // 3) Copies the requested string to the buffer
    memcpy(arrayToCopy + PORT_MESSAGE_LOG_HEADER_LENGTH, tempString, stringLengthWithNullChar);
    // As the string was created with snprintf, and it always adds an '\0' at the end of the string (https://stackoverflow.com/a/50498477),
    // there is no need to care about '\0' here. (strncpy doesn't add a \0 if the source length > given length!, but snprintf(), does!)

    // 4) Lastly, add the CRC of this packet! We want to be certain (actually, it doesn't give us 100% of error detection! Something like
    //   1/65536 certainty, assuming random errors. If it happens, is it God giving us a message? We will probably never know!!)
    uint8_t  packetLength = PORT_MESSAGE_LOG_HEADER_LENGTH + stringLengthWithNullChar;
    uint16_t crc16Var = crc16(arrayToCopy + PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET + 1, packetLength - 2); // Remember to change this sum if you changed the Port Header!
    // Explaining:
    // arrayToCopy + PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET + 1
    //      The address is the next to the MSB CRC, so we sum (+) 1!
    // packetLength - 2
    //      The length to do the CRC is the total minus the 2 bytes used in the CRC itself!

    arrayToCopy[PORT_MESSAGE_LOG_INDEX_LSB_CRC_PACKET] = LSB0(crc16Var);     // Little endian! First the Least Significant Byte!
    arrayToCopy[PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET] = MSB0(crc16Var);     // Little endian! Then the Most Significant Byte!

    return packetLength;
}