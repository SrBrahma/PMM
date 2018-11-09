/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <RH_RF95.h>                    // Our current RF module
#include "pmmConsts.h"                  // For the pinout of the RF module and RF frequency
#include "pmmTelemetry/pmmTelemetry.h"

#include "pmmTelemetry/pmmTelemetryProtocols.h"

PmmTelemetry::PmmTelemetry(): // https://stackoverflow.com/a/12927220
    mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT)
{}

int PmmTelemetry::init(PmmErrorsCentral *pmmErrorsCentral)
{
    int initCounter = 0;
    mPreviousPackageLogTransmissionMillis = mPackageLogDelayMillis = 0;

    mPmmErrorsCentral = pmmErrorsCentral;

    // Reset the priority queues
    mHighPriorityQueueStruct.actualIndex = 0;
    mHighPriorityQueueStruct.remainingItemsOnQueue = 0;
    mNormalPriorityQueueStruct.actualIndex = 0;
    mNormalPriorityQueueStruct.remainingItemsOnQueue = 0;
    mLowPriorityQueueStruct.actualIndex = 0;
    mLowPriorityQueueStruct.remainingItemsOnQueue = 0;
    mDefaultPriorityQueueStruct.actualIndex = 0;
    mDefaultPriorityQueueStruct.remainingItemsOnQueue = 0;

    //RH_RF95 mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT);

    pinMode(PMM_PIN_RFM95_RST, OUTPUT);
    digitalWrite(PMM_PIN_RFM95_RST, HIGH);

    // These delays are the default of the lora code. Maybe they aren't even needed.
    delay(10);
    digitalWrite(PMM_PIN_RFM95_RST, LOW);
    delay(10);
    digitalWrite(PMM_PIN_RFM95_RST, HIGH);
    delay(10);

    // mRf95.init() returns false if didn't initialized successfully.
    while (!mRf95.init()) // Keep trying! ...
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("PmmTelemetry: LoRa didn't initialized, attempt number "); Serial.println(initCounter);
        #endif
        if (++initCounter >= PMM_RF_INIT_MAX_TRIES) // Until counter
        {
            mPmmErrorsCentral->reportErrorByCode(ERROR_RF_INIT);
            PMM_DEBUG_PRINT("PmmTelemetry #1: LoRa didn't initialized after all these attempts.");
            return 1;
        }
    }

    // So it initialized!
    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTransmissionPower(PMM_LORA_TX_POWER, false);
    PMM_DEBUG_PRINT_MORE("PmmTelemetry [M]: LoRa initialized successfully!");

    return 0;
}



int PmmTelemetry::updateTransmission()
{
    // uint32_t tempMillis;
    pmmTelemetryQueueStructType* queueStructPtr;

    // 1) Is there any packet being sent?
    if (mRf95.isAnyPacketBeingSent())
        return 1;

    /* tempMillis = millis();
    #if PMM_DEBUG_SERIAL_MORE
        Serial.print("PmmTelemetry [M]: Time taken waiting previous package to be sent = ");
        Serial.print(millis() - tempMillis);
        Serial.println("ms.");
        Serial.print("PmmTelemetry [M]: Delay is = ");
        Serial.print(mPackageLogDelayMillis);
        Serial.println("ms.");
    #endif  */

    // 2) Is the telemetry working?
    if (!mPmmErrorsCentral->getTelemetryIsWorking())
        return 1;

    // 3) Check the queues, following the priorities. What should the PMM send now?
    if (mHighPriorityQueueStruct.remainingItemsOnQueue)
        queueStructPtr = &mHighPriorityQueueStruct;
    else if (mNormalPriorityQueueStruct.remainingItemsOnQueue)
        queueStructPtr = &mNormalPriorityQueueStruct;
    else if (mLowPriorityQueueStruct.remainingItemsOnQueue)
        queueStructPtr = &mLowPriorityQueueStruct;
    else if (mDefaultPriorityQueueStruct.remainingItemsOnQueue)
        queueStructPtr = &mDefaultPriorityQueueStruct;
    else
        return 0; // Nothing to send!

    // 4) Send it! On the future other options of telemetry may be added. This a little problem to who will work with my code on the future. 'Boa sorte', little fella.
    mRf95.send(queueStructPtr->uint8_tPtrArray[queueStructPtr->actualIndex],     // The data array
                queueStructPtr->lengthInBytesArray[queueStructPtr->actualIndex],
                &queueStructPtr->protocolsContentStructArray[queueStructPtr->actualIndex]); // The length


    // 5) After giving the order to send, increase the actualIndex of the queue, and decrease the remaining items to send on the queue.
    queueStructPtr->actualIndex++;
    if (queueStructPtr->actualIndex >= PMM_TELEMETRY_QUEUE_LENGTH)  // If the index is greater than the maximum queue index, reset it.
        queueStructPtr->actualIndex = 0;                            // (the > in >= is just to fix eventual mystical bugs.)

    queueStructPtr->remainingItemsOnQueue--;

    // 6) Done! Sent successfully!
    return 0;

} // end of updateTransmission()



// Returns 1 if received anything, else, 0.
int PmmTelemetry::updateReception()
{
    if (mRf95.receivePayloadAndStatusStruct(mReceivedPayload, &mReceivedPacketStatusStruct))
        return 1;

    return 0;
}



/* Returns the index of the new allocated item of the queue (0 ~ the maximum index of the new item in the queue). Returns -1 if not successful (queue may be full!).
 * Also, returns by reference the struct.
 * It does increase the pmmTelemetryQueueStruct.remainingItemsOnQueue at the end */
int PmmTelemetry::tryToAddToQueue(pmmTelemetryQueuePrioritiesType priority, pmmTelemetryQueueStructType *pmmTelemetryQueueStructPtr)
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

        case PMM_TELEMETRY_QUEUE_PRIORITY_DEFAULT:
            pmmTelemetryQueueStructPtr = &mDefaultPriorityQueueStruct;
            break;

        default:    // If for some mystic reason it goes wrong...
            return -1;
    } // End of switch

    // If there is no room to allocate a new item on the queue
    if (pmmTelemetryQueueStructPtr->remainingItemsOnQueue >= PMM_TELEMETRY_QUEUE_LENGTH)
        return -1;

    newItemIndex = (pmmTelemetryQueueStructPtr->actualIndex) + (pmmTelemetryQueueStructPtr->remainingItemsOnQueue);

    // If the newItemIndex is a number beyond the maximum length, turns it into a valid index. Hey, in my drawings it make sense.
    if (newItemIndex >= PMM_TELEMETRY_QUEUE_LENGTH)
        newItemIndex = PMM_TELEMETRY_QUEUE_LENGTH - PMM_TELEMETRY_QUEUE_LENGTH;

    // Now there is a new item on the queue! (Just add the remaining arguments on the function that called this one!)
    pmmTelemetryQueueStructPtr->remainingItemsOnQueue++;

    return newItemIndex; // Returns the index of the new item on the queue!
}



/* Returns 0 if added to the queue successfully, 1 ifn't. */
int PmmTelemetry::addSendToQueue(uint8_t dataArray[], uint8_t totalByteSize, telemetryProtocolsContentStructType protocolsContentStruct, pmmTelemetryQueuePrioritiesType priority)
{
    pmmTelemetryQueueStructType *pmmTelemetryQueueStructPtr = NULL; // = NULL to stop "warning: 'pmmTelemetryQueueStructPtr' is used uninitialized in this function [-Wuninitialized]"
    int newItemIndex = tryToAddToQueue(priority, pmmTelemetryQueueStructPtr);

    if (newItemIndex == -1)
        return 1;   // If no available space on the queue, return 1.

    pmmTelemetryQueueStructPtr->uint8_tPtrArray[newItemIndex] = dataArray;
    pmmTelemetryQueueStructPtr->lengthInBytesArray[newItemIndex] = totalByteSize;
    pmmTelemetryQueueStructPtr->protocolsContentStructArray[newItemIndex] = protocolsContentStruct;

    return 0;
}

// Getters
uint8_t* PmmTelemetry::getReceivedPacketArray()
{
    return mReceivedPayload;
}

telemetryPacketInfoStructType* PmmTelemetry::getReceivedPacketStatusStructPtr()
{
    return &mReceivedPacketStatusStruct;
}

// NOT USED ANYMORE. Will be removed someday. Returns 0 if added to the queue successfully, 1 ifn't.
/*
int PmmTelemetry::addSendSmartSizesToQueue(uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize, telemetryProtocolsContentStructType protocolsContentStruct, pmmTelemetryQueuePrioritiesType priority)
{
    pmmTelemetryQueueStructType *pmmTelemetryQueueStructPtr = NULL; // = NULL to stop "warning: 'pmmTelemetryQueueStructPtr' is used uninitialized in this function [-Wuninitialized]";

    int newItemIndex = tryToAddToQueue(priority, pmmTelemetryQueueStructPtr);

    if (newItemIndex == -1)
        return 1;   // If no available space on the queue, return 1.

    pmmTelemetryQueueStructPtr->sendTypeArray               [newItemIndex] = PMM_TELEMETRY_SEND_SMART_SIZES;
    pmmTelemetryQueueStructPtr->uint8_tPtrToPtrArray        [newItemIndex] = dataArrayOfPointers;
    pmmTelemetryQueueStructPtr->uint8_tPtrArray             [newItemIndex] = sizesArray;
    pmmTelemetryQueueStructPtr->numberVariablesArray        [newItemIndex] = numberVariables;
    pmmTelemetryQueueStructPtr->lengthInBytesArray          [newItemIndex] = totalByteSize;
    pmmTelemetryQueueStructPtr->protocolsContentStructArray [newItemIndex] = protocolsContentStruct;

    return 0;
}*/