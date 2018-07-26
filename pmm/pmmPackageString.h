
#ifndef PMM_PACKAGE_STRING_h
#define PMM_PACKAGE_STRING_h

#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#define PMM_PACKAGE_STRING_MAX_STRINGS 10
#define PMM_PACKAGE_STRING_MAX_STRING_LENGTH 230 // It actually could be ~240, but I don't want to do some simple maths right now.
// Actually, as I am still developing the Package String, some headers still may be added. Anyway, don't care about it.

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
};

#endif
