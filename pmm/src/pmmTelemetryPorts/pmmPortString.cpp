
#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#include <pmmTelemetryPorts/pmmPortString.h>
#include <crc16.h>
#include <byteSelection.h>

const PROGMEM char* PMM_THIS_NAME = {PMM_THIS_NAME_DEFINE};

int PmmPortString::init(uint32_t* packageLogIdPtr, uint32_t* packageLogMillisPtr, PmmTelemetry* pmmTelemetry, PmmSd* pmmSd)
{
    mPmmTelemetry = pmmTelemetry;
    mPmmSd = pmmSd;
    mPackageLogIdPtr = packageLogIdPtr;
    mPackageLogMillisPtr = packageLogMillisPtr;
    mActualNumberOfStrings = 0;
    return 0;
}

// Adds a little message header, ex: [19402ms 92112id] Parachutes Deployed
int PmmPortString::addString(char *string)
{
    snprintf(mString, PMM_PORT_STRING_MAX_STRING_LENGTH, "[%lums %luid] %s", *mPackageLogMillisPtr, *mPackageLogIdPtr, string); // Adds the time and the Package Log Id.
    
    return 0;
}

int PmmPortString::addRawString(char *string)    // Won't add the time and the Package Log Id.
{
    snprintf(mString, PMM_PORT_STRING_MAX_STRING_LENGTH, "%s", string);
    return 0;
}

bool PmmPortString::loadStringFromSd(char stringDestination[], uint16_t requestedStringId)
{

}

// As the payload length for now is 255 (with RFM95w and other modules like APC220), there is no need for now to break the String Package into packets.
uint8_t PmmPortString::getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId)
{
    // Port format is in pmmPortString.h

    uint16_t crc16Var;
    uint16_t stringLengthWithNullChar; // With \0!
    uint16_t packetLength;

    // If the requested ID is invalid.
    if (requestedStringId >= mActualNumberOfStrings)
        return 0;

    // 0!) The packet CRC will be added on the end of this function.

    // 1) Add the id of this string, and the total amount of strings - 1
    arrayToCopy[PMM_PORT_STRING_INDEX_STRING_X] = requestedStringId;
    arrayToCopy[PMM_PORT_STRING_INDEX_OF_Y_MINUS_1] = mActualNumberOfStrings - 1;

    // 2) Now adds the payload, the null-terminated string!
    stringLengthWithNullChar = strlen(mString[requestedStringId]) + 1;

    // 3) Copies the requested string to the buffer
    memcpy(arrayToCopy + PMM_PORT_STRING_HEADER_LENGTH, mString[requestedStringId], stringLengthWithNullChar);
    // As the string was created with snprintf, and it always adds an '\0' at the end of the string (https://stackoverflow.com/a/50498477),
    // there is no need to care about '\0' here. (strncpy doesn't add a \0 if the source length > given length!, but snprintf(), does!)

    // 4) Lastly, add the CRC of this packet! We want to be certain (actually, it doesn't give us 100% of error detection! Something like
    //   1/65536 certainty, assuming random errors. If it happens, is it God giving us a message? We will probably never know!!)
    packetLength = PMM_PORT_STRING_HEADER_LENGTH + stringLengthWithNullChar;
    crc16Var = crc16(arrayToCopy + PMM_PORT_STRING_INDEX_MSB_CRC_PACKET + 1, packetLength - 2); // Remember to change this sum if you changed the Port Header!
    // Explaining:
    // arrayToCopy + PMM_PORT_STRING_INDEX_MSB_CRC_PACKET + 1
    //      The address is the next to the MSB CRC, so we sum (+) 1!
    // packetLength - 2
    //      The length to do the CRC is the total minus the 2 bytes used in the CRC itself!

    arrayToCopy[PMM_PORT_STRING_INDEX_LSB_CRC_PACKET] = LSB0(crc16Var);     // Little endian! First the Least Significant Byte!
    arrayToCopy[PMM_PORT_STRING_INDEX_MSB_CRC_PACKET] = MSB0(crc16Var);     // Little endian! Then the Most Significant Byte!

    return packetLength;
}


uint8_t PmmPortString::getActualNumberOfStrings() { return mActualNumberOfStrings;}