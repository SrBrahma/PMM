/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h

#include <RH_RF95.h>
#include <pmmConsts.h>
#include <pmmErrorsCentral.h>
#include <pmmTelemetryProtocols.h>
#include "pmmPackages/pmmPorts.h"

#define PMM_TELEMETRY_QUEUE_LENGTH 8



class PmmTelemetry
{
    typedef enum
    {
        PMM_TELEMETRY_SEND,
        PMM_TELEMETRY_SEND_SMART_SIZES
    } pmmTelemetrySendType;



    typedef enum
    {
        PMM_TELEMETRY_QUEUE_PRIORITY_HIGH,
        PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL,
        PMM_TELEMETRY_QUEUE_PRIORITY_LOW,
        PMM_TELEMETRY_QUEUE_PRIORITY_DEFAULT
    } pmmTelemetryQueuePrioritiesType;



    typedef struct
    {
        pmmTelemetrySendType sendTypeArray[PMM_TELEMETRY_QUEUE_LENGTH];
        uint8_t** uint8_tPtrToPtrArray[PMM_TELEMETRY_QUEUE_LENGTH]; // Used in sendArrayOfPointersOfSmartSizes() (as the pointer of data array)
        uint8_t*  uint8_tPtrArray[PMM_TELEMETRY_QUEUE_LENGTH];      // Used in send() (as the data array) and the sendArrayOfPointersOfSmartSizes() (the sizes array)
        uint8_t   numberVariablesArray[PMM_TELEMETRY_QUEUE_LENGTH]; // Used in sendArrayOfPointersOfSmartSizes()
        uint8_t   lengthInBytesArray[PMM_TELEMETRY_QUEUE_LENGTH];   // Used in send() and the sendArrayOfPointersOfSmartSizes()

        telemetryProtocolsContentStructType protocolsContentStructArray[PMM_TELEMETRY_QUEUE_LENGTH];

        uint8_t   actualIndex;
        uint8_t   remainingItemsOnQueue; // How many items on this queue not sent yet.

    } pmmTelemetryQueueStructType;
    
public:
    PmmTelemetry();
    int init(PmmErrorsCentral *pmmErrorsCentral);
    int updateReception();
    int updateTransmission();

    int addSendToQueue(uint8_t dataArray[], uint8_t totalByteSize, telemetryProtocolsContentStructType protocolsContentStruct, pmmTelemetryQueuePrioritiesType priority);

    int addSendSmartSizesToQueue(uint8_t* dataArrayOfPointers[], uint8_t dataLengthsArray[], uint8_t numberVariables, uint8_t totalDataLength,
                                 telemetryProtocolsContentStructType protocolsContentStruct, pmmTelemetryQueuePrioritiesType priority);

    uint8_t* getReceivedPacketArray();
    telemetryPacketInfoStructType* getReceivedPacketStatusStructPtr();

    //void setPackageLogInfoReceivedFunctionPtr
    //void setPackageStringReceivedFunctionPtr
    //void setPackageRequestReceivedFunctionPtr;



private:
    uint8_t mReceivedPayload[PMM_TELEMETRY_MAX_PAYLOAD_LENGTH];
    telemetryPacketInfoStructType mReceivedPacketStatusStruct;

    PmmErrorsCentral *mPmmErrorsCentral;
    RH_RF95 mRf95;
    uint32_t mPreviousPackageLogTransmissionMillis;
    uint32_t mPackageLogDelayMillis;

    pmmTelemetryQueueStructType mHighPriorityQueueStruct;
    pmmTelemetryQueueStructType mNormalPriorityQueueStruct;
    pmmTelemetryQueueStructType mLowPriorityQueueStruct;
    pmmTelemetryQueueStructType mDefaultPriorityQueueStruct;

    int tryToAddToQueue(pmmTelemetryQueuePrioritiesType priority, pmmTelemetryQueueStructType *pmmTelemetryQueueStructPtr);



        //PmmTelemetry(); // https://stackoverflow.com/a/12927220




};


#endif
