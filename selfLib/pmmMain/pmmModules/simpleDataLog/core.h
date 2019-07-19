// By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

// You shouldn't include this normally. Do include the receiver.h or transmitter.h .

#ifndef PMM_MODULE_SIMPLE_DATA_LOG_CORE_h
#define PMM_MODULE_SIMPLE_DATA_LOG_CORE_h


#include <stdint.h>

#include "varsNames.h"
#include "pmmTelemetry/telemetry.h"      // For PMM_TLM_MAX_PAYLOAD_LENGTH
#include "pmmSd/consts.h"

#define TLM_INDEX_CRC_16_LSB        0
#define TLM_INDEX_CRC_16_MSB        1

#define TLM_INDEX_SESSION_ID        2
#define TLM_HEADER_LENGTH           3

#define TLM_CRC_LENGTH              2

#define TLM_PAYLOAD_START           TLM_HEADER_LENGTH

#define TLM_MAX_PAYLOAD_LENGTH      (PMM_TLM_MAX_PAYLOAD_LENGTH - TLM_HEADER_LENGTH)


// 000X 0-1 (1 byte)
#define TYPE_ID_UINT8      0
#define TYPE_ID_INT8       1
// 00X0 2-3 (2 bytes)
#define TYPE_ID_UINT16     2
#define TYPE_ID_INT16      3
// 0X00 4-8 (4 bytes)
#define TYPE_ID_UINT32     4
#define TYPE_ID_INT32      5
#define TYPE_ID_FLOAT      6
// X000 8-15 (8 bytes)
#define TYPE_ID_INT64      8
#define TYPE_ID_UINT64     9
#define TYPE_ID_DOUBLE     10

#define MAX_VARIABLES               50  // This must be the same value for the transmitter and the receptor.
#define MAX_STRING_LENGTH           30  // The maximum Variable String. Includes the '\0', so the max valid chars is this - 1.


// In the improbable future.... There should be one class for each dataLog group in your system.
class ModuleSimpleDataLogCore
{
public:

    static VarsNames mStr;

    ModuleSimpleDataLogCore();
    int buildCsvHeader(char destination[], int maxLength);
    int buildCsvData(char destination[], int maxLength);

protected:

    uint8_t  getVarSize(uint8_t variableType);

    uint8_t  mNumberVariables;
    char   * mVarsNameArray[MAX_VARIABLES];
    uint8_t  mVarsTypeArray[MAX_VARIABLES];
    uint8_t  mVarsSizeArray[MAX_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVarsAdrsArray[MAX_VARIABLES]; // Adrs = Address!
    uint8_t  mTotalBytes;  // The sum of all sizes.

};

#endif