
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

uint8_t PmmPackageString::getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId)
{
    // Format is ["MSTR"][CRC of the actual packet: 2B][String X of Y - 1: 2B][String]
    // void* memcpy( void *dest, const void *src, size_t count )
    // arrayToCopy[0~3] Package Header
    // arrayToCopy[4~5] CRC
    // arrayToCopy[6] String X
    // arrayToCopy[7] of a total of (Y - 1)
    // arrayToCopy[8+] The string

    uint8_t packageLength = 8; // The Package Header default length
    uint16_t crc16Var;
    if (requestedStringId >= mActualNumberOfStrings) // If the requested ID is invalid.
        return 0;



    memcpy(arrayToCopy, (void*) PMM_TELEMETRY_HEADER_STRING, 4); // Adds the Package Header.

    // 4~5 are the crc16, it's added on the next lines!

    arrayToCopy[6] = requestedStringId;
    arrayToCopy[7] = mActualNumberOfStrings - 1;

    strncpy((char*)arrayToCopy+8, mStrings[requestedStringId], PMM_PACKAGE_STRING_MAX_STRING_LENGTH);
    // As the string was created with the length limit of PMM_PACKAGE_STRING_MAX_STRING_LENGTH, there is no need
    //  to redundantly add the \0. (strncpy don't add a \0 if the source length > given length!)


    crc16Var = crc16(arrayToCopy+6, strlen(mStrings[requestedStringId]) + 2, crc16(arrayToCopy, 4));
    // It first does the CRC16 of the Package Header (length 4),
    // Then skips the Header and these 2 bytes destined to the CRC16 and do the CRC of the rest (strlen(string) + (String X of Y ( = 2)).

    arrayToCopy[4] = crc16Var & 0x0F; // Little endian! First the Least Significant Byte!
    arrayToCopy[5] = crc16Var & 0xF0 >> 8; // Little endian! Then the Most Significant Byte!

    return packageLength;
}

//PMM_THIS_NAME*/
