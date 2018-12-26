#include <stdint.h>
#include "pmmDebug.h"
#include "pmmTelemetry/pmmTelemetry.h"


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
            return 0;
    } // End of switch
}



// Returns 0 if added to the queue successfully, 1 ifn't.
int PmmTelemetry::addPacketToQueue(toBeSentPacketStructType* packetStruct, telemetryQueuePriorities priority)
{
    telemetryQueueStructType *pmmTelemetryQueueStructPtr = NULL; // = NULL to stop "warning: 'pmmTelemetryQueueStructPtr' is used uninitialized in this function [-Wuninitialized]"
    int newItemIndex = tryToAddToQueue(priority, pmmTelemetryQueueStructPtr);

    if (newItemIndex < 0)
        return 1;   // If no available space on the queue, return 1.

    addProtocolHeader (pmmTelemetryQueueStructPtr->packet[newItemIndex], packetStruct);
    addProtocolPayload(pmmTelemetryQueueStructPtr->packet[newItemIndex], packetStruct);

    return 0;
}



// Returns the index of the new allocated item of the queue (0 ~ the maximum index of the new item in the queue). Returns a negative number if
// not successful. 
// Also, returns by reference the struct.
// It does increase the pmmTelemetryQueueStruct.remainingPacketsOnQueue at the end
int PmmTelemetry::tryToAddToQueue(telemetryQueuePriorities priority, telemetryQueueStructType *pmmTelemetryQueueStructPtr)
{
    int newItemIndex;

    switch (priority)
    {
        case PMM_TELEMETRY_QUEUE_PRIORITY_HIGH:
            pmmTelemetryQueueStructPtr = &mHighPriorityQueueStruct;
            break;

        case PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL:
            pmmTelemetryQueueStructPtr = &mNormalPriorityQueueStruct;
            break;

        case PMM_TELEMETRY_QUEUE_PRIORITY_LOW:
            pmmTelemetryQueueStructPtr = &mLowPriorityQueueStruct;
            break;

        default:    // If for some mystic reason it goes wrong...
            return -1;
    } // End of switch

    // If there is no room to allocate a new item on the queue
    if (pmmTelemetryQueueStructPtr->remainingPacketsOnQueue >= PMM_TELEMETRY_QUEUE_LENGTH)
        return -1;

    newItemIndex = (pmmTelemetryQueueStructPtr->actualIndex) + (pmmTelemetryQueueStructPtr->remainingPacketsOnQueue);

    // If the newItemIndex is a number beyond the maximum length, turns it into a valid index. Hey, in my drawings it make sense.
    if (newItemIndex >= PMM_TELEMETRY_QUEUE_LENGTH)
        newItemIndex = PMM_TELEMETRY_QUEUE_LENGTH - PMM_TELEMETRY_QUEUE_LENGTH;

    // Now there is a new item on the queue! (Just add the remaining arguments on the function that called this one!)
    pmmTelemetryQueueStructPtr->remainingPacketsOnQueue++;

    return newItemIndex; // Returns the index of the new item on the queue!
}

uint8_t PmmTelemetry::getTotalPacketsRemainingOnQueue()
{
    return mHighPriorityQueueStruct.remainingPacketsOnQueue + mNormalPriorityQueueStruct.remainingPacketsOnQueue + mLowPriorityQueueStruct.remainingPacketsOnQueue;
}