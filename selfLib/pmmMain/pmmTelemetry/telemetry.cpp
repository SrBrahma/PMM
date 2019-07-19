/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#include <stdint.h>
#include <RH_RF95.h>                    // Our current RF module
#include "pmmConsts.h"                  // For the pinout of the RF module and RF frequency
#include "pmmDebug.h"
#include "pmmTelemetry/protocols.h"
#include "pmmTelemetry/telemetry.h"

#if   (PMM_LORA_SPI_CHANNEL == 0)
    #define PMM_LORA_HW_SPI     hardware_spi
#elif (PMM_LORA_SPI_CHANNEL == 1)
    #include <RHHardwareSPI1.h>
    #define PMM_LORA_HW_SPI     hardware_spi1
#elif (PMM_LORA_SPI_CHANNEL == 2)
    #include <RHHardwareSPI2.h>
    #define PMM_LORA_HW_SPI     hardware_spi2
#endif


PmmTelemetry::PmmTelemetry() : mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT, PMM_LORA_HW_SPI) {} // https://stackoverflow.com/a/12927220



int PmmTelemetry::init()
{
    pinMode(PMM_PIN_RFM95_RST, INPUT);     delay(15); // Reset pin should be left floating for >10ms, according to "7.2.1. POR" in SX1272 manual.
    pinMode(PMM_PIN_RFM95_RST, OUTPUT);
    digitalWrite(PMM_PIN_RFM95_RST, LOW);  delay( 1); // > 100uS, according to "7.2.2. Manual Reset" in SX1272 manual.
    digitalWrite(PMM_PIN_RFM95_RST, HIGH); delay(10); // > 5ms, according to "7.2.2. Manual Reset" in SX1272 manual.
                                   
    if (!mRf95.init()) { // It returns false if failed to init.
        advPrintf("LoRa failed to initialize.\n");
        mTelemetryIsWorking = false;
        return 1;
    }

    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTxPower(PMM_LORA_TX_POWER);

    mTelemetryIsWorking = true;
    tlmDebugMorePrintf("LoRa initialized successfully!\n");

    return 0;
}



int PmmTelemetry::send(PacketToBeSent* packetToBeSent) {
    if (!mTelemetryIsWorking)   return 1;
    if (!packetToBeSent)        return 2;

    uint8_t packet[PMM_TLM_MAX_PACKET_TOTAL_LENGTH]; uint8_t packetLength;

    buildPacket(packet, &packetLength, packetToBeSent);

    if (mRf95.sendWithoutHeaders(packet, packetLength))         return 3;   // Send not successful!

    #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_TLM_DEBUG_MORE && 0
        tlmDebugMorePrintf("Transmitted packet content:\n");    printArrayHex(packet, packetLength);
    #endif

    return 0;
}

int PmmTelemetry::sendIfAvailable(PacketToBeSent* packetToBeSent)   {
    if (!mTelemetryIsWorking)   return 1;
    if (!packetToBeSent)        return 2;
    // You should run the isSendAvailable() on the function that builds the packetToBeSent object, before you start building it!

    uint8_t packet[PMM_TLM_MAX_PACKET_TOTAL_LENGTH]; uint8_t packetLength;

    buildPacket(packet, &packetLength, packetToBeSent);

    // 4) Send it!
    if (mRf95.sendIfAvailable(packet, packetLength))    return 3;   // Send not successful! Maybe a previous packet still being transmitted, or Channel Activity Detected!

    #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_TLM_DEBUG_MORE && 0
        tlmDebugMorePrintf("Transmitted packet content:\n");    printArrayHex(packet, packetLength);
    #endif

    return 0;
}



// Returns true if received anything, else, false.
bool PmmTelemetry::updateReception() {
    if (mRf95.receivePayloadAndInfoStruct(&mRxPacketPhysicalLayerInfo)) {
        getRxPacketAllInfo(&mRxPacketPhysicalLayerInfo, &mRxPacketAllInfo);
        tlmDebugMorePrintf("Packet received: Protocol[%u] Source[%u] Destination[%u] Port[%u] PayloadLength[%u]\n", mRxPacketAllInfo.protocol,
                            mRxPacketAllInfo.sourceAddress, mRxPacketAllInfo.destinationAddress, mRxPacketAllInfo.port, mRxPacketAllInfo.payloadLength);
        #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_TLM_DEBUG_MORE && 0
            tlmDebugMorePrintf("Received payload content:\n"); printArrayHex(mRxPacketAllInfo.payload, mRxPacketAllInfo.payloadLength);
        #endif

        return true;
    }
    return false;
}

bool PmmTelemetry::isSendAvailable() {
    if (!mTelemetryIsWorking) return false;
    return mRf95.isSendAvailable(); }

receivedPacketAllInfoStructType* PmmTelemetry::getRxPacketAllInfoPtr()      {   return &mRxPacketAllInfo;   }