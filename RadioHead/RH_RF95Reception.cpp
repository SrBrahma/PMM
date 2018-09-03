// RH_RF95.cpp
//
// Copyright (C) 2011 Mike McCauley
// $Id: RH_RF95.cpp,v 1.18 2018/01/06 23:50:45 mikem Exp $

#include <RH_RF95.h>
#include <pmmTelemetryProtocols.h>
#include "crc16.h"
#include "byteSelection.h"


// if the given protocol is invalid, will by default use the Neo Protocol.
uint8_t RH_RF95::getProtocolHeaderLength(pmmTelemetryProtocolsType protocol)
{
    switch (protocol)
    {
        default:
        case PMM_NEO_PROTOCOL_ID:
            return PMM_NEO_PROTOCOL_HEADER_LENGTH;
    }
}



// Check the packet protocol and return the length of the header. If 0 is returned, the packet is invalid.
// It also checks if the Destination Address of the received packet is the same as the address of this system.
uint8_t RH_RF95::validateReceivedPacketAndReturnProtocolHeaderLength(uint8_t buffer[], uint8_t bufferLength)
{
    // 1) Which protocol is this packet using?
    switch(buffer[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        #if PMM_TELEMETRY_PROTOCOLS_ACCEPTS_NEO_PROTOCOL
            case PMM_NEO_PROTOCOL_ID:
                //1.1) Test the packet length
                if (mReceivedPacketBufferLength < PMM_NEO_PROTOCOL_HEADER_LENGTH)
                    return 0; // Too short to be a real message

                // 1.2) Check the Destination of this packet we received
                // If the Destination not equal to this Address and not in promiscuous mode
                if ((mPacketBuffer[PMM_NEO_PROTOCOL_INDEX_DESTINATION] != mThisAddress) && !mPromiscuousMode)
                    return 0;

                return PMM_NEO_PROTOCOL_HEADER_LENGTH;
        #endif

        default:
            return 0;
    }

}



// It assumes the packet was already validated by the validateReceivedPacketAndReturnProtocolHeaderLength() function,
// which is called in handleInterrupt() function.
void RH_RF95::getPacketInfoInStruct(uint8_t packet[], telemetryPacketInfoStructType* packetStatusStruct)
{
    // 1) Which protocol is this packet using?
    switch(packet[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        case PMM_NEO_PROTOCOL_ID:
            packetStatusStruct->sourceAddress = packet[PMM_NEO_PROTOCOL_INDEX_SOURCE];
            packetStatusStruct->destinationAddress = packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION];
            packetStatusStruct->port = packet[PMM_NEO_PROTOCOL_INDEX_PORT];
            packetStatusStruct->payloadLength = mReceivedPacketBufferLength - mReceivedPacketProtocolHeaderLength;
            packetStatusStruct->rssi = getLastRssi();
            packetStatusStruct->snr = getLastSNR();
            packetStatusStruct->invalidAutoLoraValidCrc = mInvalidAutoLoraPayloadCrc;
    }

}



// Be sure your buffer is equal or greater than RH_RF95_MAX_PACKET_LENGTH!
bool RH_RF95::receivePayloadAndStatusStruct(uint8_t* payload, telemetryPacketInfoStructType* packetStatusStruct)
{
    if (!getIsThereANewReceivedPacket())
        return false;

    if (!payload || !packetStatusStruct ) // Avoid NULL addresses
        return false;

    ATOMIC_BLOCK_START; // These exists so the packet data won't change while you are copying the data - if LoRa received another packet.
    memcpy(payload, mPacketBuffer + mReceivedPacketProtocolHeaderLength, packetStatusStruct->payloadLength);
    getPacketInfoInStruct(mPacketBuffer, packetStatusStruct);
    ATOMIC_BLOCK_END;

    clearRxBuf(); // This message accepted and cleared
    return true;
}



// Be sure your buffer is equal or greater than RH_RF95_MAX_PACKET_LENGTH!
bool RH_RF95::receivePayload(uint8_t* buffer, uint8_t* packetLength)
{
    if (!getIsThereANewReceivedPacket())
        return false;
    if (buffer && packetLength) // Avoid NULL addresses
    {
        ATOMIC_BLOCK_START; // These exists so the packet data won't change while you are copying the data - if LoRa received another packet.
        *packetLength = mReceivedPacketBufferLength - mReceivedPacketProtocolHeaderLength;
        memcpy(buffer, mPacketBuffer + mReceivedPacketProtocolHeaderLength, *packetLength);
        ATOMIC_BLOCK_END;
    }
    clearRxBuf(); // This message accepted and cleared
    return true;
}



// C++ level interrupt handler for this instance
// LORA is unusual in that it has several interrupt lines, and not a single, combined one.
// On MiniWirelessLoRa, only one of the several interrupt lines (DI0) from the RFM95 is usefuly
// connnected to the processor.
// We use this to get RxDone and TxDone interrupts
void RH_RF95::handleInterrupt()
{
    // Read the interrupt register
    uint8_t irq_flags = spiRead(RH_RF95_REG_12_IRQ_FLAGS);
    if (mMode == RH_MODE_IS_RECEIVING && irq_flags & RH_RF95_PAYLOAD_CRC_ERROR)
    {
        mInvalidAutoLoraPayloadCrc = true;
        mInvalidReceivedPacketsCounter++;
    }
    if (mMode == RH_MODE_IS_RECEIVING && irq_flags & (RH_RF95_RX_TIMEOUT))
    {
        mInvalidReceivedPacketsCounter++;
    }
    else if (mMode == RH_MODE_IS_RECEIVING && irq_flags & RH_RF95_RX_DONE)
    {
        // Have received a packet
        mReceivedPacketBufferLength = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

        // Reset the fifo read ptr to the beginning of the packet
        spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
        spiBurstRead(RH_RF95_REG_00_FIFO, mPacketBuffer, mReceivedPacketBufferLength);

        spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags

        // Remember the last signal to noise ratio, LORA mode
        // Per page 111, SX1276/77/78/79 datasheet
        mLastSNR = (int8_t)spiRead(RH_RF95_REG_19_PKT_SNR_VALUE) / 4;

        // Remember the RSSI of this packet, LORA mode
        // this is according to the doc, but is it really correct?
        // weakest receiveable signals are reported RSSI at about -66
        mLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE);


        // We have received a message.
        if((mReceivedPacketProtocolHeaderLength = validateReceivedPacketAndReturnProtocolHeaderLength(mPacketBuffer, mReceivedPacketBufferLength)));
        {
            // Adjust the RSSI, datasheet page 87
            if (mLastSNR < 0)
                mLastRssi = mLastRssi + mLastSNR;
            else
                mLastRssi = (int)mLastRssi * 16 / 15;
            if (_usingHFport)
                mLastRssi -= 157;
            else
                mLastRssi -= 164;

            mIsThereANewReceivedPacket = true;

            if (mInvalidAutoLoraPayloadCrc)
                mInvalidAutoLoraPayloadCrc = false;

            setModeIdle();
        }

    }
    else if (mMode == RH_MODE_IS_TRANSMITTING && irq_flags & RH_RF95_TX_DONE)
    {
        _txGood++;
        setModeIdle();
    }
    else if (mMode == RH_MODE_IS_DETECTING_CHANNEL_ACTIVITY_CAD && irq_flags & RH_RF95_CAD_DONE)
    {
        mCad = irq_flags & RH_RF95_CAD_DETECTED;
        setModeIdle();
    }
    // Sigh: on some processors, for some unknown reason, doing this only once does not actually
    // clear the radio's interrupt flag. So we do it twice. Why?
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
}
