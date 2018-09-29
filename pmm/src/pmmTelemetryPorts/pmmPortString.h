#ifndef PMM_PORT_STRING_h
#define PMM_PORT_STRING_h

#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#include <pmmTelemetry.h>
#include <pmmSd.h>
#include <pmmTelemetryProtocols.h>

#define PMM_PORT_STRING_INDEX_LSB_CRC_PACKET    0
#define PMM_PORT_STRING_INDEX_MSB_CRC_PACKET    1
#define PMM_PORT_STRING_INDEX_STRING_X          2
#define PMM_PORT_STRING_INDEX_OF_Y_MINUS_1      3
// =
#define PMM_PORT_STRING_HEADER_LENGTH           4
// Package String header is ["MSTR"][CRC of the current packet: 2B][String X: 1B][of Y - 1: 1B]
// [0~1] CRC
// [2] String X
// [3] of a total of (Y - 1)
// The payload is
// [4+] The string, null-terminated. (I had to make a decision - having the null char or not. I chose to have. Maybe on future I change my mind.
// As CRC exists in the package, it really doesn't change too much. On the future, it will probably exist the code for rebuilding the package from broken packages.

#define PMM_PORT_STRING_MAX_STRING_LENGTH (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_PORT_STRING_HEADER_LENGTH)




class PmmPortString
{
private:
    uint32_t* mPackageLogIdPtr;
    uint32_t* mPackageLogMillisPtr; // To share the same millis as the Package Log
    PmmTelemetry* mPmmTelemetry;
    PmmSd* mPmmSd;
    char mStrings[PMM_PORT_STRING_MAX_STRING_LENGTH]; // As teensy 3.6 have 256kB RAM, I don't care too much about big arrays.
    uint8_t mActualNumberOfStrings;

public:
    int init(uint32_t* packageLogIdPtr, uint32_t* packageLogMillisPtr, PmmTelemetry* pmmTelemetry, PmmSd* pmmSd);
    int addString(char *string);
    int addRawString(char *string);

    void receivedPackageString(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);

    uint8_t getActualNumberOfStrings();
    uint8_t getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId);
};

#endif
