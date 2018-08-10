/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmTelemetry.h>
#include <RH_RF95.h>
#include <pmmConsts.h>

PmmTelemetry::PmmTelemetry(): // https://stackoverflow.com/a/12927220
    mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT)
{}

int PmmTelemetry::init(PmmErrorsCentral *pmmErrorsCentral)
{
    int initCounter = 0;
    mPreviousPackageLogTransmissionMillis = mPackageLogDelayMillis = 0;

    mPmmErrorsCentral = pmmErrorsCentral;

    /* Reset the priority queues */
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

    /* These delays are the default of the lora code. Maybe they aren't even needed. */
    delay(100);
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

    /* So it initialized! */

    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTxPower(PMM_LORA_TX_POWER, false);
    PMM_DEBUG_PRINT_MORE("PmmTelemetry: LoRa initialized successfully!");
    return 0;
}

int PmmTelemetry::updateTransmission()
{
    // uint32_t tempMillis;
    pmmTelemetryQueueStructType* queueStructPtr;

    // 1) Is there any packet being sent?
    if (mRf95.isAnyPacketBeingSentRH_RF95())
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
    // else
        // Error here! Invalid priority!

    // 4) Which kind of send is it? A normal send(), or a sendOfSmartSizes? (Or a future kind of send!)
    // Then, send!
    switch(queueStructPtr->sendTypeArray[queueStructPtr->actualIndex])
    {
        case PMM_TELEMETRY_SEND:
            mRf95.send(queueStructPtr->uint8_tPtrArray[queueStructPtr->actualIndex],     // The data array
                       queueStructPtr->lengthInBytesArray[queueStructPtr->actualIndex]); // The length
            break;
        case PMM_TELEMETRY_SEND_SMART_SIZES:
            mRf95.sendArrayOfPointersOfSmartSizes(
                queueStructPtr->uint8_tPtrToPtrArray[queueStructPtr->actualIndex], // The array of data array
                queueStructPtr->uint8_tPtrArray[queueStructPtr->actualIndex],      // The sizes array
                queueStructPtr->numberVariablesArray[queueStructPtr->actualIndex], // The number of variables
                queueStructPtr->lengthInBytesArray[queueStructPtr->actualIndex]);  // The total byte size
            break;
        default:
            // Error here! Invalid type! Treat it on the future!
            break;

    } // End of switch

    //

    // 5) After giving the order to send, increase the actualIndex of the queue, and decrease the remaining items to send on the queue.
    queueStructPtr->actualIndex++;
    if (queueStructPtr->actualIndex >= PMM_TELEMETRY_QUEUE_LENGTH)  // If the index is greater than the maximum queue index, reset it.
        queueStructPtr->actualIndex = 0;                            // (the > in >= is just to fix eventual mystical bugs.)

    queueStructPtr->remainingItemsOnQueue--;

    // 6) Done! Sent successfully!
    return 0;
} // end of updateTransmission()



int PmmTelemetry::updateReception()
{
    if (mRf95.recv2(mRfPayload))
    {
        #if PMM_IS_PDA
        if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_TYPE_LOG, 4)) // MLOG
        {
            mPmmPackageLog->
            return 0;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_TYPE_STRING, 4)) // MSTR
        {
            // save in txt

            return 0;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_TYPE_LOG_INFO, 4)) // MLIN
        {

            return 0;
        }
        else
            return 0;
        #endif

    }
    else
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

    newItemIndex = pmmTelemetryQueueStructPtr->actualIndex + pmmTelemetryQueueStructPtr->remainingItemsOnQueue;

    // If the newItemIndex is a number beyond the maximum length, turns it into a valid index. Hey, in my drawings it make sense.
    if (newItemIndex >= PMM_TELEMETRY_QUEUE_LENGTH)
        newItemIndex = PMM_TELEMETRY_QUEUE_LENGTH - PMM_TELEMETRY_QUEUE_LENGTH;

    // Now there is a new item on the queue! (Just add the remaining arguments on the function that called this one!)
    pmmTelemetryQueueStructPtr->remainingItemsOnQueue++;

    return newItemIndex; // Returns the index of the new item on the queue!
}




/* Returns 0 if added to the queue successfully, 1 ifn't. */
int PmmTelemetry::addSendToQueue(uint8_t dataArray[], uint8_t totalByteSize, pmmTelemetryQueuePrioritiesType priority)
{
    pmmTelemetryQueueStructType *pmmTelemetryQueueStructPtr;
    int newItemIndex = tryToAddToQueue(priority, pmmTelemetryQueueStructPtr);

    if (newItemIndex == -1)
        return 1;   // If no available space on the queue, return 1.


    pmmTelemetryQueueStructPtr->sendTypeArray[newItemIndex] = PMM_TELEMETRY_SEND;
    // pmmTelemetryQueueStructPtr->uint8_tPtrToPtrArray[newItemIndex]; // Not used in this function!
    pmmTelemetryQueueStructPtr->uint8_tPtrArray[newItemIndex] = dataArray;
    //pmmTelemetryQueueStructPtr->numberVariablesArray[newItemIndex]; // Not used in this function!
    pmmTelemetryQueueStructPtr->lengthInBytesArray[newItemIndex] = totalByteSize;

    return 0;
}




/* Returns 0 if added to the queue successfully, 1 ifn't. */
int PmmTelemetry::addSendSmartSizesToQueue(uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize, pmmTelemetryQueuePrioritiesType priority)
{
    pmmTelemetryQueueStructType *pmmTelemetryQueueStructPtr;
    int newItemIndex = tryToAddToQueue(priority, pmmTelemetryQueueStructPtr);

    if (newItemIndex == -1)
        return 1;   // If no available space on the queue, return 1.


    pmmTelemetryQueueStructPtr->sendTypeArray[newItemIndex] = PMM_TELEMETRY_SEND_SMART_SIZES;
    pmmTelemetryQueueStructPtr->uint8_tPtrToPtrArray[newItemIndex] = dataArrayOfPointers;
    pmmTelemetryQueueStructPtr->uint8_tPtrArray[newItemIndex] = sizesArray;
    pmmTelemetryQueueStructPtr->numberVariablesArray[newItemIndex] = numberVariables;
    pmmTelemetryQueueStructPtr->lengthInBytesArray[newItemIndex] = totalByteSize;
    return 0;
}
/* for the future
int
PmmTelemetry::setTxPower(int value)
{
    value <= 5? value = 5;
    value >= 23? value = 23;

    mRf95.setTxPower(value, false);
    return 0;
} */
