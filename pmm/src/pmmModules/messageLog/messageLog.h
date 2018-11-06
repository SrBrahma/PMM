#ifndef PMM_PORT_STRING_h
#define PMM_PORT_STRING_h

#include <stdint.h> // for uint32_t

#include "pmmTelemetry/pmmTelemetry.h"
#include "pmmSd/pmmSd.h"

#define PMM_PORT_MESSAGE_LOG_INDEX_LSB_CRC_PACKET   0
#define PMM_PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET   1
#define PMM_PORT_MESSAGE_LOG_INDEX_STRING_X         2
#define PMM_PORT_MESSAGE_LOG_INDEX_OF_Y_MINUS_1     3
// Total header length is equal to...
#define PMM_PORT_STRING_HEADER_LENGTH               4

#define PMM_PACKAGE_MESSAGE_LOG_MAX_STRING_LENGTH (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_PORT_STRING_HEADER_LENGTH)




class PmmModuleMessageLog
{

public:
    int init(uint32_t* packageLogIdPtr, uint32_t* packageLogMillisPtr, PmmTelemetry* pmmTelemetry, PmmSd* pmmSd);
    int addString(char *string); // For now, all added strings will be saved on SD and sent by telemetry.
    int addRawString(char *string); // For now, all added strings will be saved on SD and sent by telemetry.

    void receivedPackageString(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);

    uint8_t getActualNumberOfStrings();
    uint8_t getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId);

    int loadStringFromSd(char stringDestination[], uint16_t requestedStringId);

private:
    uint32_t* mPackageLogIdPtr;
    uint32_t* mPackageLogMillisPtr; // To share the same millis as the Package Log
    PmmTelemetry* mPmmTelemetry;
    PmmSd* mPmmSd;
    char mString[PMM_PACKAGE_MESSAGE_LOG_MAX_STRING_LENGTH];
    uint8_t mActualNumberOfStrings;


};

#endif
