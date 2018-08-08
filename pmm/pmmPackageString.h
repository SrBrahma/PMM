
#ifndef PMM_PACKAGE_STRING_h
#define PMM_PACKAGE_STRING_h

#include <stdint.h> // for uint32_t
#include <pmmConsts.h>

#define PMM_PACKAGE_STRING_MAX_STRINGS 10

#define PMM_TELEMETRY_PACKAGE_STRING_HEADER_LENGTH 8
// Package String header is ["MSTR"][CRC of the actual packet: 2B][String X: 1B][of Y - 1: 1B]
// [0~3] Package Header
// [4~5] CRC
// [6] String X
// [7] of a total of (Y - 1)
// The payload is
// [8+] The string, null-terminated. (I had to make a decision - having the null char or not. I chose to have. Maybe on future I change my mind.
// As CRC exists in the package, it really doesn't change too much. On the future, it will probably exist the code for rebuilding the package from broken packages.

#define PMM_PACKAGE_STRING_MAX_STRING_LENGTH (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_TELEMETRY_PACKAGE_STRING_HEADER_LENGTH)



const PROGMEM char* PMM_THIS_NAME = {"Aurora"};

class PmmPackageString
{
private:
    uint32_t* mPackageLogIdPtr;
    char mStrings[PMM_PACKAGE_STRING_MAX_STRINGS][PMM_PACKAGE_STRING_MAX_STRING_LENGTH]; // As teensy 3.6 have 256kB RAM, I don't care too much about big arrays.
    uint8_t mActualNumberOfStrings;

public:
    int init(uint32_t *packageLogIdPtr);
    int addString(char *string);
    int addRawString(char *string);

    void receivedPackageString(uint8_t* packetArray, uint8_t packetSize);
    
    uint8_t getActualNumberOfStrings();
    uint8_t getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId);
};

#endif
