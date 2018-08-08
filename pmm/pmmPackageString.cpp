
#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#include <pmmPackageString.h>
#include <crc16.h>

int PmmPackageString::init(uint32_t *packageLogIdPtr)
{
    mPackageLogIdPtr = packageLogIdPtr;
    mActualNumberOfStrings = 0;
    return 0;
}

int PmmPackageString::addString(char *string)
{
    snprintf(mStrings[mActualNumberOfStrings++], PMM_PACKAGE_STRING_MAX_STRING_LENGTH, "%lums %luid %s: %s", millis(), *mPackageLogIdPtr, PMM_THIS_NAME, string);       // Adds the time and the Package Log Id.
    return 0;
}

int PmmPackageString::addRawString(char *string)    // Won't add the time and the Package Log Id.
{
    snprintf(mStrings[mActualNumberOfStrings++], PMM_PACKAGE_STRING_MAX_STRING_LENGTH, "%s", string);
    return 0;
}

uint8_t PmmPackageString::getActualNumberOfStrings() { return mActualNumberOfStrings;}

// As the payload length for now is 255 (with RFM95w and other modules like APC220), there is no need for now to break the String Package into packets.
uint8_t PmmPackageString::getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId)
{
    // Format is ["MSTR"][CRC of the actual packet: 2B][String X of Y - 1: 2B][String]
    // void* memcpy( void *dest, const void *src, size_t count )
    // arrayToCopy[0~3] Package Header
    // arrayToCopy[4~5] CRC
    // arrayToCopy[6] String X
    // arrayToCopy[7] of a total of (Y - 1)
    // arrayToCopy[8+] The string

    uint16_t crc16Var;
    uint16_t stringLengthWithNullChar; // With \0!
    uint16_t packetLength;

    // If the requested ID is invalid.
    if (requestedStringId >= mActualNumberOfStrings)
        return 0;

    // 1) Add the packet header!
    // 1.a) First add the Package Header.
    memcpy(arrayToCopy, PMM_TELEMETRY_HEADER_TYPE_STRING, 4);

    // 1.b) Then the temporary CRC of the package
    arrayToCopy[4] = 0; // CRC least significant byte as 0 to calculate the CRC of the entire array
    arrayToCopy[5] = 0; // CRC most significant byte as 0 to calculate the CRC of the entire array

    // 1.c) Then the id of this string, and the total amount of strings - 1
    arrayToCopy[6] = requestedStringId;
    arrayToCopy[7] = mActualNumberOfStrings - 1;


    // 2) Now adds the payload, the null-terminated string!
    stringLengthWithNullChar = strlen(mStrings[requestedStringId]) + 1;

    packetLength = PMM_TELEMETRY_PACKAGE_STRING_HEADER_LENGTH + stringLengthWithNullChar;

    memcpy(arrayToCopy + PMM_TELEMETRY_PACKAGE_STRING_HEADER_LENGTH, mStrings[requestedStringId], stringLengthWithNullChar);
    // As the string was created with snprintf, and it always adds an '\0' at the end of the string (https://stackoverflow.com/a/50498477),
    // there is no need to care about '\0' here. (strncpy doesn't add a \0 if the source length > given length!, but snprintf(), does!)


    crc16Var = crc16(arrayToCopy, packetLength); // See why we temporarily made the CRC as 0?

    arrayToCopy[4] = crc16Var & 0x0F;      // Little endian! First the Least Significant Byte!
    arrayToCopy[5] = crc16Var & 0xF0 >> 8; // Little endian! Then the Most Significant Byte!

    return packetLength;
}

//PMM_THIS_NAME*/
