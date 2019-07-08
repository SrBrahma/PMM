/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_TLM_h
#define PMM_TLM_h

#include <RH_RF95.h>                            // Our current RF module!
#include "pmmTelemetry/protocols.h" // For the PMM_NEO_PROTOCOL_HEADER_LENGTH define




#define PMM_TLM_QUEUE_LENGTH            8
#define PMM_RF_INIT_MAX_TRIES           10

#define PMM_TLM_SIMULATE_RECEPTION      0       // Be sure to disable it when using for real!





class PmmTelemetry
{
public:
    PmmTelemetry();

    int init();

    int updateReception();
    int updateTransmission();

    uint8_t availablePositionsInQueue(telemetryQueuePriorities priority);

    // As the telemetry will usually be slow, I did a queue system. To send a packet, you must use this.
    int addPacketToQueue(PacketToBeSent* packetToBeSent);

    uint8_t getTotalPacketsRemainingOnQueue();

    receivedPacketAllInfoStructType* getReceivedPacketAllInfoStructPtr();

    // This below shouldn't be normally used. Used in debugging.
    int setTelemetryConfig(RH_RF95::ModemConfigChoice index);
    int sendIfAvailableDebug(uint8_t data[], uint8_t length);
    int isPacketBeingSent();

private:
    typedef struct
    {
        telemetryQueuePriorities thisPriority;  // For getting the priority by the struct.
        uint8_t   packet      [PMM_TLM_QUEUE_LENGTH][PMM_TLM_MAX_PACKET_TOTAL_LENGTH];  // The packet data, ready to be sent.
        uint8_t   packetLength[PMM_TLM_QUEUE_LENGTH];
        unsigned* feedback    [PMM_TLM_QUEUE_LENGTH]; // A variable that will be set to 0 when the packet is added to the queue, and to 1 when it is sent.
        uint8_t   actualIndex;
        uint8_t   remainingPacketsOnQueue; // How many items on this queue not sent yet.
    } telemetryQueueStructType;

    int         getQueueStruct(telemetryQueuePriorities priority, telemetryQueueStructType** pmmTelemetryQueueStructPtr);
    const char* getQueuePriorityString(telemetryQueuePriorities priority);

    RH_RF95  mRf95;

    unsigned mTelemetryIsWorking;

    receivedPacketPhysicalLayerInfoStructType   mReceivedPacketPhysicalLayerInfoStruct;
    receivedPacketAllInfoStructType             mReceivedPacketAllInfoStruct          ;

    telemetryQueueStructType mHighPriorityQueueStruct  ;
    telemetryQueueStructType mNormalPriorityQueueStruct;
    telemetryQueueStructType mLowPriorityQueueStruct   ;

    #if PMM_TLM_SIMULATE_RECEPTION
        int                                         mNewSimulatedPacket;
    #endif
};


#endif
