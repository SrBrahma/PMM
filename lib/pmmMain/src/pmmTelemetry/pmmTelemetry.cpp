/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <RH_RF95.h>                    // Our current RF module
#include "pmmConsts.h"                  // For the pinout of the RF module and RF frequency
#include "pmmDebug.h"
#include "pmmTelemetry/pmmTelemetry.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h"

PmmTelemetry::PmmTelemetry()
    : mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT) // https://stackoverflow.com/a/12927220
{}



int PmmTelemetry::init()
{
    int initCounter = 0;

    // Reset the priority queues
    mHighPriorityQueueStruct.actualIndex = 0;
    mHighPriorityQueueStruct.remainingPacketsOnQueue = 0;
    mNormalPriorityQueueStruct.actualIndex = 0;
    mNormalPriorityQueueStruct.remainingPacketsOnQueue = 0;
    mLowPriorityQueueStruct.actualIndex = 0;
    mLowPriorityQueueStruct.remainingPacketsOnQueue = 0;

    pinMode(PMM_PIN_RFM95_RST, OUTPUT);     // (does this make the pin floating?)
    delay(15);                              // Reset pin should be left floating for >10ms, according to "7.2.1. POR" in SX1272 manual.
    digitalWrite(PMM_PIN_RFM95_RST, LOW);
    delay(1);                               // > 100uS, according to "7.2.2. Manual Reset" in SX1272 manual.
    digitalWrite(PMM_PIN_RFM95_RST, HIGH);
    delay(10);                               // >5ms, according to "7.2.2. Manual Reset" in SX1272 manual.

    // mRf95.init() returns false if didn't initialized successfully.
    while (!mRf95.init()) // Keep trying! ...
    {
        initCounter++;
        PMM_DEBUG_ADV_PRINT("Fail at initialize, attempt ") PMM_DEBUG_PRINT(initCounter) PMM_DEBUG_PRINT(" of ")
        PMM_DEBUG_PRINT(PMM_RF_INIT_MAX_TRIES) PMM_DEBUG_PRINTLN(".")

        if (initCounter >= PMM_RF_INIT_MAX_TRIES) // Until counter
        {
            mTelemetryIsWorking = 0;
            PMM_DEBUG_ADV_PRINT("Max attempts reached, LoRa didn't initialize.");
            return 1;
        }
    }

    // So it initialized!
    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTransmissionPower(PMM_LORA_TX_POWER, false);

    mTelemetryIsWorking = 1;
    PMM_DEBUG_MORE_PRINTLN("PmmTelemetry [M]: LoRa initialized successfully!");

    return 0;
}



int PmmTelemetry::updateTransmission()
{
    telemetryQueueStructType* queueStructPtr;

    // 1) Is the telemetry working?
    if (!mTelemetryIsWorking)
        return 1;

 
    // 2) Is there any packet being sent?
    if (mRf95.isAnyPacketBeingSent())
        return 2;


    // 3) Check the queues, following the priorities. What should the PMM send now?
    if (mHighPriorityQueueStruct.remainingPacketsOnQueue)
        queueStructPtr = &mHighPriorityQueueStruct;
    else if (mNormalPriorityQueueStruct.remainingPacketsOnQueue)
        queueStructPtr = &mNormalPriorityQueueStruct;
    else if (mLowPriorityQueueStruct.remainingPacketsOnQueue)
        queueStructPtr = &mLowPriorityQueueStruct;
    else
        return 0; // Nothing to send!


    // 4) Send it!
    if (mRf95.sendIfAvailable(queueStructPtr->packet[queueStructPtr->actualIndex], queueStructPtr->packetLength[queueStructPtr->actualIndex]))
        return 3;   // Send not successful! Maybe a previous packet still being transmitted, or Channel Activity Detected!


    // 5) After giving the order to send, increase the actualIndex of the queue, and decrease the remaining items to send on the queue.
    queueStructPtr->actualIndex++;
    if (queueStructPtr->actualIndex >= PMM_TELEMETRY_QUEUE_LENGTH)  // If the index is greater than the maximum queue index, reset it.
        queueStructPtr->actualIndex = 0;                            // (the > in >= is just to fix eventual mystical bugs.)

    queueStructPtr->remainingPacketsOnQueue--;

    // 6) Done! Sent successfully!
    return 0;

} // end of updateTransmission()



// Returns 1 if received anything, else, 0.
int PmmTelemetry::updateReception()
{
    if (mRf95.receivePayloadAndInfoStruct(mReceivedPacket, mReceivedPacketPhysicalLayerInfoStructPtr));
    {
        getReceivedPacketAllInfoStruct(mReceivedPacket, mReceivedPacketPhysicalLayerInfoStructPtr, mReceivedPacketAllInfoStructPtr);
        return 1;
    }
    return 0;
}



uint8_t PmmTelemetry::availablePositionsInQueue(telemetryQueuePrioritiesType priority)
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
int PmmTelemetry::addPacketToQueue(toBeSentPacketStructType* packetStruct, telemetryQueuePrioritiesType priority)
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
int PmmTelemetry::tryToAddToQueue(telemetryQueuePrioritiesType priority, telemetryQueueStructType *pmmTelemetryQueueStructPtr)
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



receivedPacketAllInfoStructType* PmmTelemetry::getReceivedPacketStatusStructPtr()
{
    return mReceivedPacketAllInfoStructPtr;
}

uint8_t PmmTelemetry::getTotalPacketsRemainingOnQueue()
{
    return mHighPriorityQueueStruct.remainingPacketsOnQueue + mNormalPriorityQueueStruct.remainingPacketsOnQueue + mLowPriorityQueueStruct.remainingPacketsOnQueue;
}
