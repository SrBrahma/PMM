/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TELEMETRY_h
#define PMM_TELEMETRY_h

#include <RH_RF95.h>                            // Our current RF module!
#include "pmmTelemetry/pmmTelemetryProtocols.h" // For the PMM_NEO_PROTOCOL_HEADER_LENGTH define


#define PMM_TELEMETRY_MAX_PACKET_TOTAL_LENGTH   255 // Max LoRa Packet Size!

#define PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH     (PMM_TELEMETRY_MAX_PACKET_TOTAL_LENGTH - PMM_NEO_PROTOCOL_HEADER_LENGTH)

#define PMM_TELEMETRY_MAX_PAYLOAD_LENGTH        PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH // Since this is currently the only protocol.

#define PMM_TELEMETRY_QUEUE_LENGTH              8


class PmmTelemetry
{

public:

    typedef enum
    {
        PMM_TELEMETRY_QUEUE_PRIORITY_HIGH,
        PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL,
        PMM_TELEMETRY_QUEUE_PRIORITY_LOW
    } telemetryQueuePrioritiesType;

    PmmTelemetry();

    int init();

    int updateReception();
    int updateTransmission();

    int addPacketToQueue(uint8_t dataArray[], toBeSentPacketStructType toBeSentPacketInfoStruct, telemetryQueuePrioritiesType priority);

    uint8_t* getReceivedPacketArray();
    
    receivedPacketAllInfoStructType* getReceivedPacketStatusStructPtr();



private:

    typedef struct
    {
        toBeSentPacketStructType toBeSentPacketInfoStruct[PMM_TELEMETRY_QUEUE_LENGTH];
        unsigned*                feedback                [PMM_TELEMETRY_QUEUE_LENGTH]; // A variable that will be set to 0 when the packet is added to the queue, and to 1 when it is sent.
        uint8_t                  actualIndex;
        uint8_t                  remainingItemsOnQueue; // How many items on this queue not sent yet.
    } telemetryQueueStructType;


    RH_RF95  mRf95;

    unsigned mTelemetryIsWorking;

    uint8_t  mReceivedPacket[PMM_TELEMETRY_MAX_PACKET_TOTAL_LENGTH];

    receivedPacketAllInfoStructType*           mReceivedPacketAllInfoStructPtr          ;
    receivedPacketPhysicalLayerInfoStructType* mReceivedPacketPhysicalLayerInfoStructPtr;

    telemetryQueueStructType mHighPriorityQueueStruct  ;
    telemetryQueueStructType mNormalPriorityQueueStruct;
    telemetryQueueStructType mLowPriorityQueueStruct   ;

    int tryToAddToQueue(telemetryQueuePrioritiesType priority, telemetryQueueStructType *pmmTelemetryQueueStructPtr);

};


#endif
