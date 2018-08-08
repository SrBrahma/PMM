/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h


#include <Arduino.h>
#include <RH_RF95.h>
#include <pmmConsts.h>
#include <pmmErrorsCentral.h>

#define PMM_TELEMETRY_BUFFER_LENGTH 8

typedef enum
{
    PMM_TELEMETRY_SEND,
    PMM_TELEMETRY_SEND_SMART_SIZES,
    PMM_TELEMETRY_SEND_TYPES_TOTAL // The total number of send types
} pmmTelemetrySendType;

typedef enum
{
    PMM_TELEMETRY_BUFFER_PRIORITY_HIGH,
    PMM_TELEMETRY_BUFFER_PRIORITY_NORMAL,
    PMM_TELEMETRY_BUFFER_PRIORITY_LOW,
    PMM_TELEMETRY_BUFFER_PRIORITY_DEFAULT,
    PMM_TELEMETRY_BUFFER_PRIORITY_TOTAL
} pmmTelemetryBufferPrioritiesType;

typedef struct
{
    pmmTelemetrySendType sendTypeArray;
    uint8_t** uint8_tPtrToPtrArray[PMM_TELEMETRY_BUFFER_LENGTH]; // Used in sendArrayOfPointersOfSmartSizes() (as the pointer of data array)
    uint8_t*  uint8_tPtrArray[PMM_TELEMETRY_BUFFER_LENGTH];      // Used in send() (as the data array) and the sendArrayOfPointersOfSmartSizes() (the sizes array)
    uint8_t   numberVariables;                                   // Used in sendArrayOfPointersOfSmartSizes()
    uint8_t   lengthInBytes;                                     // Used in send() and the sendArrayOfPointersOfSmartSizes()
    uint8_t   actualIndex;
    uint8_t   startingIndex;                                     
} pmmTelemetryBufferType;

class PmmTelemetry
{
    //PmmTelemetry(); // https://stackoverflow.com/a/12927220

private:
    uint8_t mRfPayload[PMM_TELEMETRY_MAX_PAYLOAD_LENGTH];
    PmmErrorsCentral *mPmmErrorsCentral;
    RH_RF95 mRf95;
    uint32_t mPreviousPackageLogTransmissionMillis;
    uint32_t mPackageLogDelayMillis;


public:
    PmmTelemetry();
    int init(PmmErrorsCentral *pmmErrorsCentral);
    int update();
    int updateReception();
    int updateTransmission();
    int addSendToBuffer();
};


#endif
