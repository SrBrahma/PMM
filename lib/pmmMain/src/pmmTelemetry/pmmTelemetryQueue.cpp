#include <stdint.h>

#include "pmmDebug.h"
#include "pmmTelemetry/pmmTelemetry.h"



// Returns 0 if added to the queue successfully, 1 ifn't.
int PmmTelemetry::addPacketToQueue(toBeSentPacketStructType* packetStruct, telemetryQueuePriorities priority)
{
    if (!packetStruct)
        return 1;

    // If there is no room to allocate a new item on the queue
    if (!availablePositionsInQueue(priority))
        return 2;

    telemetryQueueStructType* queueStructPtr = NULL; // = NULL to stop "warning: 'queueStructPtr' is used uninitialized in this function [-Wuninitialized]"

    getQueueStruct(priority, &queueStructPtr);
    int newItemIndex = queueStructPtr->actualIndex + queueStructPtr->remainingPacketsOnQueue;

    // If the new index is beyond the maximum index, it means it needs to 'circulate' the queue.
    if (newItemIndex >= PMM_TELEMETRY_QUEUE_LENGTH)
        newItemIndex -= PMM_TELEMETRY_QUEUE_LENGTH;

    // Now there is a new item on the queue!
    queueStructPtr->remainingPacketsOnQueue++;

    addProtocolHeader (queueStructPtr->packet[newItemIndex], &queueStructPtr->packetLength[newItemIndex], packetStruct);
    addProtocolPayload(queueStructPtr->packet[newItemIndex], &queueStructPtr->packetLength[newItemIndex], packetStruct);

    tlmDebugMorePrintf("New packet added to <%s> priority queue, on position <%u>. Packet content is:\n", getQueuePriorityString(priority), newItemIndex)
    #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_TLM_DEBUG_MORE
        // printHexArray(queueStructPtr->packet[newItemIndex], queueStructPtr->packetLength[newItemIndex]);
    #endif


    return 0;
}


uint8_t PmmTelemetry::availablePositionsInQueue(telemetryQueuePriorities priority)
{
    switch (priority)
    {
        case PMM_TELEMETRY_QUEUE_PRIORITY_HIGH:
            return PMM_TELEMETRY_QUEUE_LENGTH - mHighPriorityQueueStruct.remainingPacketsOnQueue;

        case PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL:
            return PMM_TELEMETRY_QUEUE_LENGTH - mNormalPriorityQueueStruct.remainingPacketsOnQueue;

        case PMM_TELEMETRY_QUEUE_PRIORITY_LOW:
            return PMM_TELEMETRY_QUEUE_LENGTH - mLowPriorityQueueStruct.remainingPacketsOnQueue;

        default:    // If for some mystic reason it goes wrong...
            advPrintf("Invalid priority.\n")
            return 0;
    } // End of switch
}


uint8_t PmmTelemetry::getTotalPacketsRemainingOnQueue()
{
    return mHighPriorityQueueStruct.remainingPacketsOnQueue + mNormalPriorityQueueStruct.remainingPacketsOnQueue + mLowPriorityQueueStruct.remainingPacketsOnQueue;
}


int PmmTelemetry::getQueueStruct(telemetryQueuePriorities priority, telemetryQueueStructType** pmmTelemetryQueueStructPtr)
{
    switch (priority)
    {
        case PMM_TELEMETRY_QUEUE_PRIORITY_HIGH:
            *pmmTelemetryQueueStructPtr = &mHighPriorityQueueStruct;
            break;

        case PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL:
            *pmmTelemetryQueueStructPtr = &mNormalPriorityQueueStruct;
            break;

        case PMM_TELEMETRY_QUEUE_PRIORITY_LOW:
            *pmmTelemetryQueueStructPtr = &mLowPriorityQueueStruct;
            break;

        default:    // If for some mystic reason it goes wrong...
            advPrintf("Invalid priority.\n")
            return 1;
    }
    return 0;
}

const char* PmmTelemetry::getQueuePriorityString(telemetryQueuePriorities priority)
{
    const PROGMEM char* HIGH_PRIORITY_STRING    = "high";
    const PROGMEM char* NORMAL_PRIORITY_STRING  = "normal";
    const PROGMEM char* LOW_PRIORITY_STRING     = "low";
    const PROGMEM char* INVALID_PRIORITY_STRING = "INVALID";

    switch (priority)
    {
        case PMM_TELEMETRY_QUEUE_PRIORITY_HIGH:
            return HIGH_PRIORITY_STRING;

        case PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL:
            return NORMAL_PRIORITY_STRING;

        case PMM_TELEMETRY_QUEUE_PRIORITY_LOW:
            return LOW_PRIORITY_STRING;

        default:    // If for some mystic reason it goes wrong...
            advPrintf("Invalid priority.\n")
            return INVALID_PRIORITY_STRING;
    }
}