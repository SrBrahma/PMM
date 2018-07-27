
#ifndef PMM_PACKAGE_STRING_h
#define PMM_PACKAGE_STRING_h

#include <stdint.h> // for uint32_t
#include <pmmConsts.h>

#define PMM_PACKAGE_STRING_MAX_STRINGS 10
#define PMM_PACKAGE_STRING_MAX_STRING_LENGTH (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - 8) // 8 is the length of the Header of the Package String

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
    
    uint8_t getActualNumberOfStrings();
    uint8_t getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId);
};

#endif
