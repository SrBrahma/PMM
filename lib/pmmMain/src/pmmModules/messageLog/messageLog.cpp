
#include <stdint.h> // for uint32_t
#include <pmmConsts.h>
#include "pmmModules/messageLog/messageLog.h" // antes era: #include <pmmPackages/pmmPortString.h>
#include <crc.h>
#include <byteSelection.h>

int PmmModuleMessageLog::init(uint32_t* mainLoopCounterPtr, PmmTelemetry* pmmTelemetry, PmmSd* pmmSd)
{
    mPmmTelemetryPtr    = pmmTelemetry;
    mPmmSdPtr           = pmmSd;
    mMainLoopCounterPtr = mainLoopCounterPtr;

    mStringTransmissionCounter = 0;
    mCurrentlNumberOfStrings     = 0;

    return 0;
}

// Adds a little message header, ex: [19402ms 92112id] Parachutes Deployed
int PmmModuleMessageLog::addString(char *string)
{
    char buffer[MESSAGE_LOG_MAX_STRING_LENGTH_TELEMETRY];
    snprintf(buffer, MESSAGE_LOG_MAX_STRING_LENGTH_TELEMETRY, "[%lums %luid] %s", millis(), *mMainLoopCounterPtr, string); // Adds the time and the Package Log Id.
    
    addRawString(buffer);

    return 0;
}

int PmmModuleMessageLog::addRawString(char *string)    // Won't add the time and the Package Log Id.
{
    char buffer[MESSAGE_LOG_MAX_STRING_LENGTH_TELEMETRY];
    snprintf(buffer, MESSAGE_LOG_MAX_STRING_LENGTH_TELEMETRY, "%s", string);
    return 0;
}

uint8_t PmmModuleMessageLog::getActualNumberOfStrings() { return mCurrentlNumberOfStrings;}