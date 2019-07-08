/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <stdint.h>
#include <RH_RF95.h>                    // Our current RF module
#include "pmmConsts.h"                  // For the pinout of the RF module and RF frequency
#include "pmmDebug.h"
#include "pmmTelemetry/protocols.h"
#include "pmmTelemetry/telemetry.h"


PmmTelemetry::PmmTelemetry()
    : mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT) // https://stackoverflow.com/a/12927220
{
    mHighPriorityQueueStruct.thisPriority   = PMM_TLM_QUEUE_PRIORITY_HIGH  ;
    mNormalPriorityQueueStruct.thisPriority = PMM_TLM_QUEUE_PRIORITY_NORMAL;
    mLowPriorityQueueStruct.thisPriority    = PMM_TLM_QUEUE_PRIORITY_LOW   ;
}



int PmmTelemetry::init()
{
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

    int initCounter = 0;
    // mRf95.init() returns false if didn't initialized successfully.
    while (!mRf95.init()) // Keep trying! ...
    {
        initCounter++;
        advPrintf("Fail at initialize, attempt %i of %i.", initCounter,PMM_RF_INIT_MAX_TRIES)

        if (initCounter >= PMM_RF_INIT_MAX_TRIES) // Until counter
        {
            mTelemetryIsWorking = 0;
            advPrintf("Max attempts reached, LoRa didn't initialize.");
            return 1;
        }
    }

    // So it initialized!
    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTxPower(PMM_LORA_TX_POWER);

    mTelemetryIsWorking = 1;
    tlmDebugMorePrintf("LoRa initialized successfully!\n");

    return 0;
}


int PmmTelemetry::updateTransmission()
{
    // 1) Is the telemetry working?
    if (!mTelemetryIsWorking)
        return 1;

    telemetryQueueStructType* queueStructPtr;

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
    int returnVal;
    if ((returnVal = mRf95.sendIfAvailable(queueStructPtr->packet[queueStructPtr->actualIndex], queueStructPtr->packetLength[queueStructPtr->actualIndex])))
    {
        // tlmDebugMorePrintf("Return value of sendIfAvailable() is <%i>.\n", returnVal)
        return 2;   // Send not successful! Maybe a previous packet still being transmitted, or Channel Activity Detected!
    }

    tlmDebugMorePrintf("Packet of <%s> priority and from position <%u> successfully sent.\n", getQueuePriorityString(queueStructPtr->thisPriority), queueStructPtr->actualIndex);

    #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_TLM_DEBUG_MORE && 0
        tlmDebugMorePrintf("Transmitted packet content:\n");
        printArrayHex(queueStructPtr->packet[queueStructPtr->actualIndex], queueStructPtr->packetLength[queueStructPtr->actualIndex]);
    #endif

    #if PMM_TLM_SIMULATE_RECEPTION
        memcpy(mReceivedPacketPhysicalLayerInfoStruct.packet, queueStructPtr->packet[queueStructPtr->actualIndex], queueStructPtr->packetLength[queueStructPtr->actualIndex]);
        mReceivedPacketPhysicalLayerInfoStruct.packetLength = queueStructPtr->packetLength[queueStructPtr->actualIndex];
        mNewSimulatedPacket = 1;
    #endif

    // 5) After giving the order to send, increase the actualIndex of the queue, and decrease the remaining items to send on the queue.
    queueStructPtr->actualIndex++;
    if (queueStructPtr->actualIndex >= PMM_TLM_QUEUE_LENGTH)  // If the index is greater than the maximum queue index, reset it.
        queueStructPtr->actualIndex = 0;

    queueStructPtr->remainingPacketsOnQueue--;

    // 6) Done! Sent successfully!
    return 0;

} // end of updateTransmission()



// Returns 1 if received anything, else, 0.
int PmmTelemetry::updateReception()
{
    #if !PMM_TLM_SIMULATE_RECEPTION
        if (mRf95.receivePayloadAndInfoStruct(&mReceivedPacketPhysicalLayerInfoStruct))
        {

    #else
        if (mNewSimulatedPacket)
        {
            mNewSimulatedPacket = 0;

    #endif

            getReceivedPacketAllInfoStruct(&mReceivedPacketPhysicalLayerInfoStruct, &mReceivedPacketAllInfoStruct);
            tlmDebugMorePrintf("Packet received: Protocol[%u] Source[%u] Destination[%u] Port[%u] PayloadLength[%u]\n",
                                mReceivedPacketAllInfoStruct.protocol, mReceivedPacketAllInfoStruct.sourceAddress,
                                mReceivedPacketAllInfoStruct.destinationAddress, mReceivedPacketAllInfoStruct.port,
                                mReceivedPacketAllInfoStruct.payloadLength);
            #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_TLM_DEBUG_MORE && 0
                tlmDebugMorePrintf("Received payload content:\n");
                printArrayHex(mReceivedPacketAllInfoStruct.payload, mReceivedPacketAllInfoStruct.payloadLength);
            #endif

            return 1;
        }
        return 0;
}



receivedPacketAllInfoStructType* PmmTelemetry::getReceivedPacketAllInfoStructPtr()
{
    return &mReceivedPacketAllInfoStruct;
}


int PmmTelemetry::sendIfAvailableDebug(uint8_t data[], uint8_t length)
{
    return mRf95.sendIfAvailable(data, length);
}
int PmmTelemetry::isPacketBeingSent()
{
    return mRf95.isPacketBeingSent();
}
int PmmTelemetry::setTelemetryConfig(RH_RF95::ModemConfigChoice index)
{
    return mRf95.setModemConfig(index);
}