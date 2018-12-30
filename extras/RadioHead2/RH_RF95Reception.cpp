// RH_RF95.cpp
//
// Copyright (C) 2011 Mike McCauley
// $Id: RH_RF95.cpp,v 1.18 2018/01/06 23:50:45 mikem Exp $

#include <RH_RF95.h>
#include "crc.h"
#include "byteSelection.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h"



// Be sure your buffer is equal or greater than RH_RF95_MAX_PACKET_LENGTH!
// This version retuns by reference a receivedPacketPhysicalLayerInfoStructType, which includes the packetLength, the SNR and the RSSI.
// Returns true when received a "valid" packet (valid protocol, valid addresses, valid crc).
bool RH_RF95::receivePayloadAndInfoStruct(uint8_t* payload, receivedPacketPhysicalLayerInfoStructType* receivedPacketPhysicalLayerInfoStruct)
{
    if (!getIsThereANewReceivedPacket())
        return false;

    if (!payload || !receivedPacketPhysicalLayerInfoStruct) // Avoid NULL addresses
        return false;

    ATOMIC_BLOCK_START; // These exists so the packet data won't change while you are copying the data - if LoRa received another packet.

    memcpy(payload, mPacketBuffer, mReceivedPacketLength);
    receivedPacketPhysicalLayerInfoStruct->packetLength = mReceivedPacketLength;
    receivedPacketPhysicalLayerInfoStruct->snr          = mLastSNR;
    receivedPacketPhysicalLayerInfoStruct->rssi         = mLastRssi;

    ATOMIC_BLOCK_END;

    clearRxBuf(); // This message accepted and cleared

    return true;
}

// Be sure your buffer is equal or greater than RH_RF95_MAX_PACKET_LENGTH!
// This version returns by reference only the packetLength.
bool RH_RF95::receivePayload(uint8_t packet[], uint8_t* packetLength)
{
    if (!getIsThereANewReceivedPacket())
        return false;
    
    if (!packet || !packetLength) // Avoid NULL addresses
        return false;

    ATOMIC_BLOCK_START; // These exists so the packet data won't change while you are copying the data - if LoRa received another packet.

    *packetLength = mReceivedPacketLength;
    memcpy(packet, mPacketBuffer, *packetLength);
    
    ATOMIC_BLOCK_END;

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

    if (mMode == RH_MODE_IS_RECEIVING && (irq_flags & RH_RF95_PAYLOAD_CRC_ERROR))
    {
        mInvalidAutoLoraPayloadCrc = true;
        mInvalidReceivedPacketsCounter++;
    }

    else if (mMode == RH_MODE_IS_RECEIVING && (irq_flags & RH_RF95_RX_TIMEOUT))
    {
        mInvalidReceivedPacketsCounter++;
    }

    else if (mMode == RH_MODE_IS_RECEIVING && (irq_flags & RH_RF95_RX_DONE))
    {
        // Have received a packet
        mReceivedPacketLength = spiRead(RH_RF95_REG_13_RX_NB_BYTES);

        // Reset the fifo read ptr to the beginning of the packet
        spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, spiRead(RH_RF95_REG_10_FIFO_RX_CURRENT_ADDR));
        spiBurstRead(RH_RF95_REG_00_FIFO, mPacketBuffer, mReceivedPacketLength);

        // spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags HB: this looks useless. at the end this already happens. TWICE.

        // Remember the last signal to noise ratio, LORA mode
        // Per page 111, SX1276/77/78/79 datasheet
        mLastSNR = (int8_t)spiRead(RH_RF95_REG_19_PKT_SNR_VALUE) / 4;

        // Remember the RSSI of this packet, LORA mode
        // this is according to the doc, but is it really correct?
        // weakest receiveable signals are reported RSSI at about -66
        mLastRssi = spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE);

        // Validates the received message on the PMM Protocols.
        if(!validateReceivedPacket(mPacketBuffer, mReceivedPacketLength, mThisAddress, mPromiscuousMode));    // The validate function returns non-zero when errors found.
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

            mIsThereANewReceivedPacket = true;  // We have a valid packet!

            if (mInvalidAutoLoraPayloadCrc)         // If it was true, now it's false. It's checked on the first if in this function.
                mInvalidAutoLoraPayloadCrc = false;
        }
        
        setModeIdle();

    }

    else if (mMode == RH_MODE_IS_TRANSMITTING && (irq_flags & RH_RF95_TX_DONE))
    {
        mSuccessfulTransmittedPacketsCounter++;
        setModeIdle();
    }

    else if (mMode == RH_MODE_IS_DETECTING_CHANNEL_ACTIVITY_CAD && (irq_flags & RH_RF95_CAD_DONE))
    {
        mCad = irq_flags & RH_RF95_CAD_DETECTED;
        setModeIdle();
    }

    // Sigh: on some processors, for some unknown reason, doing this only once does not actually
    // clear the radio's interrupt flag. So we do it twice. Why?
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
    spiWrite(RH_RF95_REG_12_IRQ_FLAGS, 0xff); // Clear all IRQ flags
}