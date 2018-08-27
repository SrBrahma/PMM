// RH_RF95.cpp
//
// Copyright (C) 2011 Mike McCauley
// $Id: RH_RF95.cpp,v 1.18 2018/01/06 23:50:45 mikem Exp $

#include <RH_RF95.h>
#include <pmmTelemetryProtocols.h>
#include "generalLibs/crc16.h"
#include "generalLibs/byteSelection.h"

// Interrupt vectors for the 3 Arduino interrupt pins
// Each interrupt can be handled by a different instance of RH_RF95, allowing you to have
// 2 or more LORAs per Arduino
RH_RF95* RH_RF95::_deviceForInterrupt[RH_RF95_NUM_INTERRUPTS] = {0, 0, 0};
uint8_t RH_RF95::_interruptCount = 0; // Index into _deviceForInterrupt for next device

// These are indexed by the values of ModemConfigChoice
// Stored in flash (program) memory to save SRAM
PROGMEM static const RH_RF95::ModemConfig MODEM_CONFIG_TABLE[] =
{
    //  1d,     1e,      26
    { 0x72,   0x74,    0x04}, // Bw125Cr45Sf128 (the chip default), AGC enabled
    { 0x92,   0x74,    0x04}, // Bw500Cr45Sf128, AGC enabled
    { 0x48,   0x94,    0x04}, // Bw31_25Cr48Sf512, AGC enabled
    { 0x78,   0xc4,    0x0c}, // Bw125Cr48Sf4096, AGC enabled

};

RH_RF95::RH_RF95(uint8_t slaveSelectPin, uint8_t interruptPin, RHGenericSPI& spi)
    :
    RHSPIDriver(slaveSelectPin, spi),
    _rxBufValid(0)
{
    _interruptPin = interruptPin;
    _myInterruptIndex = 0xff; // Not allocated yet
}

bool RH_RF95::init()
{
    if (!RHSPIDriver::init())
        return false;

    // Determine the interrupt number that corresponds to the interruptPin
    int interruptNumber = digitalPinToInterrupt(_interruptPin);
    if (interruptNumber == NOT_AN_INTERRUPT)
        return false;

    #ifdef RH_ATTACHINTERRUPT_TAKES_PIN_NUMBER
        interruptNumber = _interruptPin;
    #endif

    // Tell the low level SPI interface we will use SPI within this interrupt
    spiUsingInterrupt(interruptNumber);

    // No way to check the device type :-(

    // Set sleep mode, so we can also set LORA mode:
    spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE);
    delay(10); // Wait for sleep mode to take over from say, CAD
    // Check we are in sleep mode, with LORA set
    if (spiRead(RH_RF95_REG_01_OP_MODE) != (RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE))
    {
        // Serial.println(spiRead(RH_RF95_REG_01_OP_MODE), HEX);
        return false; // No device present?
    }

    // Add by Adrien van den Bossche <vandenbo@univ-tlse2.fr> for Teensy
    // ARM M4 requires the below. else pin interrupt doesn't work properly.
    // On all other platforms, its innocuous, belt and braces
    pinMode(_interruptPin, INPUT);

    // Set up interrupt handler
    // Since there are a limited number of interrupt glue functions isr*() available,
    // we can only support a limited number of devices simultaneously
    // ON some devices, notably most Arduinos, the interrupt pin passed in is actuallt the
    // interrupt number. You have to figure out the interruptnumber-to-interruptpin mapping
    // yourself based on knwledge of what Arduino board you are running on.
    if (_myInterruptIndex == 0xff)
    {
        // First run, no interrupt allocated yet
        if (_interruptCount <= RH_RF95_NUM_INTERRUPTS)
            _myInterruptIndex = _interruptCount++;
        else
            return false; // Too many devices, not enough interrupt vectors
    }
    _deviceForInterrupt[_myInterruptIndex] = this;
    if (_myInterruptIndex == 0)
        attachInterrupt(interruptNumber, isr0, RISING);
    else if (_myInterruptIndex == 1)
        attachInterrupt(interruptNumber, isr1, RISING);
    else if (_myInterruptIndex == 2)
        attachInterrupt(interruptNumber, isr2, RISING);
    else
        return false; // Too many devices, not enough interrupt vectors

    // Set up FIFO
    // We configure so that we can use the entire 256 byte FIFO for either receive
    // or transmit, but not both at the same time
    spiWrite(RH_RF95_REG_0E_FIFO_TX_BASE_ADDR, 0);
    spiWrite(RH_RF95_REG_0F_FIFO_RX_BASE_ADDR, 0);

    // Packet format is preamble + explicit-header + payload + crc
    // Explicit Header Mode
    // payload is TO + FROM + ID + FLAGS + message data
    // RX mode is implmented with RXCONTINUOUS
    // max message data length is 255 - 4 = 251 octets

    setModeIdle();

    // Set up default configuration
    // No Sync Words in LORA mode.
    setModemConfig(Bw125Cr45Sf128); // Radio default
//    setModemConfig(Bw125Cr48Sf4096); // slow and reliable?
    setPreambleLength(8); // Default is 8
    // An innocuous ISM frequency, same as RF22's
    setFrequency(434.0);
    // Lowish power
    setTxPower(13);

    return true;
}

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


// Adds the corresponding header depending on the protocol. May add some informations, like CRC, depending on protocol.
// It assumes you already checked the total length of the packet to be sent.
void RH_RF95::addProtocolHeader(uint8_t payload[], uint8_t payloadLength, pmmTelemetryProtocolsType protocol)
{
    // 1) Which protocol are we using?
    switch (protocol)
    {
        // 1.1) Will by default (if the given protocol is invalid) use the Neo Protocol (is the only available at the current time!)
        default:
        case PMM_NEO_PROTOCOL_ID:
            // 1.1.1) Write the Protocol ID
            spiWrite(RH_RF95_REG_00_FIFO, PMM_NEO_PROTOCOL_ID);
            // 1.1.2) Write the Source Address
            spiWrite(RH_RF95_REG_00_FIFO, mTransmissionSourceAddress);
            // 1.1.3) Write the Destination Address
            spiWrite(RH_RF95_REG_00_FIFO, mTransmissionDestinationAddress);

            // 1.1.4) Calculates the packet CRC
            uint16_t tempCrc16;
            tempCrc16 = crc16SingleByte(PMM_NEO_PROTOCOL_ID);
            tempCrc16 = crc16SingleByte(mTransmissionSourceAddress, tempCrc16);
            tempCrc16 = crc16SingleByte(mTransmissionDestinationAddress, tempCrc16);
            tempCrc16 = crc16(payload, payloadLength, tempCrc16);

            // 1.1.5) Write the CRC16, in Little Endian.
            spiWrite(RH_RF95_REG_00_FIFO, LSB0(tempCrc16));
            spiWrite(RH_RF95_REG_00_FIFO, LSB1(tempCrc16));

            break;
    }
}

// Check the packet protocol header and return its (header) length. If 0 is returned, the packet is invalid.
uint8_t RH_RF95::validateReceivedPacketAndReturnProtocolHeaderLength(uint8_t buffer[], uint8_t bufferLength)
{
    // 1) Which protocol is this packet using?
    switch(buffer[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        #if PMM_TELEMETRY_PROTOCOLS_ACCEPTS_NEO_PROTOCOL
            case PMM_NEO_PROTOCOL_ID:
                if (mReceivedPacketBufferLength < PMM_NEO_PROTOCOL_HEADER_LENGTH)
                    return 0; // Too short to be a real message

                // 1) First check the Destination of this packet we received
                // If the Destination not equal to this Address and not in promiscuous mode
                if ((mPacketBuffer[PMM_NEO_PROTOCOL_INDEX_DESTINATION] != mThisAddress) && !mPromiscuousMode)
                    return 0;

                return PMM_NEO_PROTOCOL_HEADER_LENGTH;
        #endif

        default:
            return 0;
    }

}


// Be sure your buffer is equal or greater than RH_RF95_MAX_PACKET_LENGTH!
bool RH_RF95::receivePayload(uint8_t* buffer, uint8_t* packetLength)
{
    if (!available())
        return false;
    if (buffer && packetLength) // Avoid NULL addresses
    {
        ATOMIC_BLOCK_START; // I don't know why there is an interrupt block here! RadioHead default.
        *packetLength = mReceivedPacketBufferLength - mReceivedPacketProtocolHeaderLength;
        memcpy(buffer, mPacketBuffer + mReceivedPacketProtocolHeaderLength, *packetLength);
        ATOMIC_BLOCK_END;
    }
    clearRxBuf(); // This message accepted and cleared
    return true;
}


// Won't automatically add any header on the packet! Know what you are doing!
bool RH_RF95::sendRaw(const uint8_t* data, uint8_t packetLength)
{
    if (packetLength > RH_RF95_MAX_PACKET_LENGTH || !packetLength) // If invalid length
        return false;

    if (!data)          // If data is NULL
        return false;

    waitPacketSent(); // Make sure we dont interrupt an outgoing message
    setModeIdle();

    if (!waitCAD())
        return false;  // Check channel activity

    // Position at the beginning of the FIFO
    spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0);

    // The message data
    spiBurstWrite(RH_RF95_REG_00_FIFO, data, packetLength);
    spiWrite(RH_RF95_REG_22_PAYLOAD_LENGTH, packetLength);

    setModeTransmission(); // Start the transmitter
    // when Tx is done, interruptHandler will fire and radio mode will return to STANDBY
    return true;
}

// protocol default is PMM_NEO_PROTOCOL_ID
bool RH_RF95::send(const uint8_t* data, uint8_t packetLength, pmmTelemetryProtocolsType protocol)
{
    // 1) Check if the length is invalid
    if ((packetLength + getProtocolHeaderLength(protocol) > RH_RF95_MAX_PACKET_LENGTH) | !packetLength)
        return false;

    waitPacketSent(); // Make sure we dont interrupt an outgoing message
    setModeIdle();

    if (!waitCAD())
        return false;  // Check channel activity

    // Position at the beginning of the FIFO
    spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0);

    // The headers


    // The message data
    spiBurstWrite(RH_RF95_REG_00_FIFO, data, packetLength);
    spiWrite(RH_RF95_REG_22_PAYLOAD_LENGTH, packetLength + RH_RF95_HEADER_LEN);

    setModeTransmission(); // Start the transmitter
    // when Tx is done, interruptHandler will fire and radio mode will return to STANDBY
    return true;
}

/* By Henrique Bruno, UFRJ Minerva Rockets.
*/
bool RH_RF95::sendArrayOfPointersOfSmartSizes(uint8_t** data, uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize, pmmTelemetryProtocolsType protocol, uint8_t )
{
    // 1) Check if the length is invalid
    if ((totalByteSize + getProtocolHeaderLength(protocol) > RH_RF95_MAX_PACKET_LENGTH) | !totalByteSize)
        return false;

    //unsigned long timeTo = millis();
    waitPacketSent(); // Make sure we dont interrupt an outgoing message
    setModeIdle();

    if (!waitCAD())
        return false;  // Check channel activity
    //Serial.print(millis() - timeTo); Serial.println("ms idle");
    // Position at the beginning of the FIFO
    spiWrite(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0);

    addProtocolHeader(uint8_t payload[], uint8_t payloadLength, pmmTelemetryProtocolsType protocol)

    // The message data
    spiBurstWriteArrayOfPointersOfSmartSizes(RH_RF95_REG_00_FIFO, data, sizesArray, numberVariables);

    spiWrite(RH_RF95_REG_22_PAYLOAD_LENGTH, totalByteSize + RH_RF95_HEADER_LEN);

    setModeTransmission(); // Start the transmitter
    // when Tx is done, interruptHandler will fire and radio mode will return to STANDBY
    return true;
}

bool RH_RF95::setFrequency(float centre)
{
    // Frf = FRF / FSTEP
    uint32_t frf = (centre * 1000000.0) / RH_RF95_FSTEP;
    spiWrite(RH_RF95_REG_06_FRF_MSB, (frf >> 16) & 0xff);
    spiWrite(RH_RF95_REG_07_FRF_MID, (frf >> 8) & 0xff);
    spiWrite(RH_RF95_REG_08_FRF_LSB, frf & 0xff);
    _usingHFport = (centre >= 779.0);

    return true;
}

void RH_RF95::setModeIdle()
{
    if (mMode != RH_MODE_IS_IDLE)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_STDBY);
        mMode = RH_MODE_IS_IDLE;
    }
}

bool RH_RF95::sleep()
{
    if (mMode != RH_MODE_IS_SLEEPING)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_SLEEP);
        mMode = RH_MODE_IS_SLEEPING;
    }
    return true;
}

void RH_RF95::setModeReception()
{
    if (mMode != RH_MODE_IS_RECEIVING)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_RXCONTINUOUS);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x00); // Interrupt on RxDone
        mMode = RH_MODE_IS_RECEIVING;
    }
}

void RH_RF95::setModeTransmission()
{
    if (mMode != RH_MODE_IS_TRANSMITTING)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_TX);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x40); // Interrupt on TxDone
        mMode = RH_MODE_IS_TRANSMITTING;
    }
}

void RH_RF95::setTxPower(int8_t power, bool useRFO)
{
    // Sigh, different behaviours depending on whther the module use PA_BOOST or the RFO pin
    // for the transmitter output
    if (useRFO)
    {
        if (power > 14)
            power = 14;
        if (power < -1)
            power = -1;
        spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_MAX_POWER | (power + 1));
    }
    else
    {
        if (power > 23)
            power = 23;
        if (power < 5)
            power = 5;

        // For RH_RF95_PA_DAC_ENABLE, manual says '+20dBm on PA_BOOST when OutputPower=0xf'
        // RH_RF95_PA_DAC_ENABLE actually adds about 3dBm to all power levels. We will us it
        // for 21, 22 and 23dBm
        if (power > 20)
        {
            spiWrite(RH_RF95_REG_4D_PA_DAC, RH_RF95_PA_DAC_ENABLE);
            power -= 3;
        }
        else
        {
            spiWrite(RH_RF95_REG_4D_PA_DAC, RH_RF95_PA_DAC_DISABLE);
        }

        // RFM95/96/97/98 does not have RFO pins connected to anything. Only PA_BOOST
        // pin is connected, so must use PA_BOOST
        // Pout = 2 + OutputPower.
        // The documentation is pretty confusing on this topic: PaSelect says the max power is 20dBm,
        // but OutputPower claims it would be 17dBm.
        // My measurements show 20dBm is correct
        spiWrite(RH_RF95_REG_09_PA_CONFIG, RH_RF95_PA_SELECT | (power - 5));
    }
}

// Sets registers from a canned modem configuration structure
void RH_RF95::setModemRegisters(const ModemConfig* config)
{
    spiWrite(RH_RF95_REG_1D_MODEM_CONFIG1,       config->reg_1d);
    spiWrite(RH_RF95_REG_1E_MODEM_CONFIG2,       config->reg_1e);
    spiWrite(RH_RF95_REG_26_MODEM_CONFIG3,       config->reg_26);
}

// Set one of the canned FSK Modem configs
// Returns true if its a valid choice
bool RH_RF95::setModemConfig(ModemConfigChoice index)
{
    if (index > (signed int)(sizeof(MODEM_CONFIG_TABLE) / sizeof(ModemConfig)))
        return false;

    ModemConfig cfg;
    memcpy_P(&cfg, &MODEM_CONFIG_TABLE[index], sizeof(RH_RF95::ModemConfig));
    setModemRegisters(&cfg);

    return true;
}

void RH_RF95::setPreambleLength(uint16_t bytes)
{
    spiWrite(RH_RF95_REG_20_PREAMBLE_MSB, bytes >> 8);
    spiWrite(RH_RF95_REG_21_PREAMBLE_LSB, bytes & 0xff);
}

bool RH_RF95::isChannelActive()
{
    // Set mode RH_MODE_IS_DETECTING_CHANNEL_ACTIVITY_CAD
    if (mMode != RH_MODE_IS_DETECTING_CHANNEL_ACTIVITY_CAD)
    {
        spiWrite(RH_RF95_REG_01_OP_MODE, RH_RF95_MODE_CAD);
        spiWrite(RH_RF95_REG_40_DIO_MAPPING1, 0x80); // Interrupt on CadDone
        mMode = RH_MODE_IS_DETECTING_CHANNEL_ACTIVITY_CAD;
    }

    while (mMode == RH_MODE_IS_DETECTING_CHANNEL_ACTIVITY_CAD)
        YIELD;

    return mCad;
}

bool RH_RF95::available()
{
    if (mMode == RH_MODE_IS_TRANSMITTING)
        return false;
    setModeReception();
    return _rxBufValid; // Will be set by the interrupt handler when a good message is received
}

void RH_RF95::clearRxBuf()
{
    ATOMIC_BLOCK_START;
    _rxBufValid = false;
    mReceivedPacketBufferLength = 0;
    ATOMIC_BLOCK_END;
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
    if (mMode == RH_MODE_IS_RECEIVING && irq_flags & (RH_RF95_RX_TIMEOUT | RH_RF95_PAYLOAD_CRC_ERROR))
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
        // Adjust the RSSI, datasheet page 87
        if (mLastSNR < 0)
            mLastRssi = mLastRssi + mLastSNR;
        else
            mLastRssi = (int)mLastRssi * 16 / 15;
        if (_usingHFport)
            mLastRssi -= 157;
        else
            mLastRssi -= 164;

        // We have received a message.
        if(validateReceivedPacketAndReturnProtocolHeaderLength(mPacketBuffer, mReceivedPacketBufferLength));
        {
            _rxBufValid = true;
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

// Getters!
int RH_RF95::getLastSNR()
{
    return mLastSNR;
}

// These are low level functions that call the interrupt handler for the correct
// instance of RH_RF95.
// 3 interrupts allows us to have 3 different devices
void RH_RF95::isr0()
{
    if (_deviceForInterrupt[0])
    _deviceForInterrupt[0]->handleInterrupt();
}
void RH_RF95::isr1()
{
    if (_deviceForInterrupt[1])
    _deviceForInterrupt[1]->handleInterrupt();
}
void RH_RF95::isr2()
{
    if (_deviceForInterrupt[2])
    _deviceForInterrupt[2]->handleInterrupt();
}

void RH_RF95::enableTCXO()
{
    while ((spiRead(RH_RF95_REG_4B_TCXO) & RH_RF95_TCXO_TCXO_INPUT_ON) != RH_RF95_TCXO_TCXO_INPUT_ON)
    {
        sleep();
        spiWrite(RH_RF95_REG_4B_TCXO, (spiRead(RH_RF95_REG_4B_TCXO) | RH_RF95_TCXO_TCXO_INPUT_ON));
    }
}

// From section 4.1.5 of SX1276/77/78/79
// Ferror = FreqError * 2**24 * BW / Fxtal / 500
int RH_RF95::frequencyError()
{
    int32_t freqerror = 0;

    // Convert 2.5 bytes (5 nibbles, 20 bits) to 32 bit signed int
    // Caution: some C compilers make errors with eg:
    // freqerror = spiRead(RH_RF95_REG_28_FEI_MSB) << 16
    // so we go more carefully.
    freqerror = spiRead(RH_RF95_REG_28_FEI_MSB);
    freqerror <<= 8;
    freqerror |= spiRead(RH_RF95_REG_29_FEI_MID);
    freqerror <<= 8;
    freqerror |= spiRead(RH_RF95_REG_2A_FEI_LSB);
    // Sign extension into top 3 nibbles
    if (freqerror & 0x80000)
    freqerror |= 0xfff00000;

    int error = 0; // In hertz
    float bw_tab[] = {7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250, 500};
    uint8_t bwindex = spiRead(RH_RF95_REG_1D_MODEM_CONFIG1) >> 4;
    if (bwindex < (sizeof(bw_tab) / sizeof(float)))
    error = (float)freqerror * bw_tab[bwindex] * ((float)(1L << 24) / (float)RH_RF95_FXOSC / 500.0);
    // else not defined

    return error;
}
