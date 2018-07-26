
#include <stdint.h> // for uint32_t
#include <pmmPackageString.h>
#include <pmmConsts.h>

int PmmPackageString::init(uint32_t *packageLogIdPtr)
{
    mPackageLogIdPtr = packageLogIdPtr;
    mActualNumberOfStrings = 0;
    return 0;
}
int PmmPackageString::addString(char *string)
{
    snprintf(mStrings[mActualNumberOfStrings++], PMM_TELEMETRY_MAX_PAYLOAD_LENGTH, "%ums %uid %s: %s");       // Adds the time and the Package Log Id.
}
int PmmPackageString::addRawString(char *string);    // Won't add the time and the Package Log Id.

//PMM_THIS_NAME
