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
    mRf95.setTxPower(10, false);
    PMM_DEBUG_PRINT_MORE("PmmTelemetry: LoRa initialized successfully!");
    return 0;
}

int PmmTelemetry::updateTransmission()
{
    uint32_t tempMillis;

    if (millis() >= mPreviousPackageLogTransmissionMillis + mPackageLogDelayMillis)
    {
        tempMillis = millis();

        mRf95.waitPacketSent();

        mPackageLogDelayMillis += millis() - tempMillis;

        #if PMM_DEBUG_SERIAL_MORE
            Serial.print("PmmTelemetry [M]: Time taken waiting previous package to be sent = ");
            Serial.print(millis() - tempMillis);
            Serial.println("ms.");

            Serial.print("PmmTelemetry [M]: Delay is = ");
            Serial.print(mPackageLogDelayMillis);
            Serial.println("ms.");
        #endif


        if (mPmmErrorsCentral->getTelemetryIsWorking())
        {
            //mPmmErrorsCentral->blinkRfLED(HIGH);
            //PMM_DEBUG_PRINT("SENDING!");
            //mRf95.sendArrayOfPointersOfSmartSizes(mPmmPackageLog->getVariableAddressArray(), mPmmPackageLog->getVariableSizeArray(),
                                                  //mPmmPackageLog->getNumberOfVariables(), mPmmPackageLog->getPackageLogSizeInBytes());
            //mRf95.send(arr, 4);

            //PMM_DEBUG_PRINT("SENT 2");

            //mPmmErrorsCentral->blinkRfLED(LOW);
        }
        mPreviousPackageLogTransmissionMillis = millis();
        return 1;
    }
    return 0;
}

int PmmTelemetry::updateReception()
{
    if (mRf95.recv2(mRfPayload))
    {
        #if PMM_IS_PDA
        if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_TYPE_LOG, 4)) // MLOG
        {
            mPmmPackageLog->
            return 1;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_TYPE_STRING, 4)) // MSTR
        {
            // save in txt

            return 1;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_TYPE_LOG_INFO, 4)) // MLIN
        {
            return 1;
        }
        else
            return 0;
        #endif
    }
    else
        return 0;
}

/* Returns the index of the new allocated item of the queue (0 ~ the maximum index of the new item in the queue). Returns -1 if not successful.
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
