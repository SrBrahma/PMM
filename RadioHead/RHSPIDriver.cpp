// RHSPIDriver.cpp
//
// Copyright (C) 2014 Mike McCauley
// $Id: RHSPIDriver.cpp,v 1.11 2017/11/06 00:04:08 mikem Exp $

#include <RHSPIDriver.h>

RHSPIDriver::RHSPIDriver(uint8_t slaveSelectPin, RHGenericSPI& spi)
    :
    _spi(spi),
    _slaveSelectPin(slaveSelectPin)
{
}

bool RHSPIDriver::init()
{
    // start the SPI library with the default speeds etc:
    // On Arduino Due this defaults to SPI1 on the central group of 6 SPI pins
    _spi.begin();

    // Initialise the slave select pin
    // On Maple, this must be _after_ spi.begin
    pinMode(_slaveSelectPin, OUTPUT);
    digitalWrite(_slaveSelectPin, HIGH);

    delay(100);
    return true;
}

uint8_t RHSPIDriver::spiRead(uint8_t reg)
{
    uint8_t val;
    ATOMIC_BLOCK_START;
    digitalWrite(_slaveSelectPin, LOW);
    _spi.transfer(reg & ~RH_SPI_WRITE_MASK); // Send the address with the write mask off
    val = _spi.transfer(0); // The written value is ignored, reg value is read
    digitalWrite(_slaveSelectPin, HIGH);
    ATOMIC_BLOCK_END;
    return val;
}

uint8_t RHSPIDriver::spiWrite(uint8_t reg, uint8_t val)
{
    uint8_t status = 0;
    ATOMIC_BLOCK_START;
    _spi.beginTransaction();
    digitalWrite(_slaveSelectPin, LOW);
    status = _spi.transfer(reg | RH_SPI_WRITE_MASK); // Send the address with the write mask on
    _spi.transfer(val); // New value follows
    digitalWrite(_slaveSelectPin, HIGH);
    _spi.endTransaction();
    ATOMIC_BLOCK_END;
    return status;
}

uint8_t RHSPIDriver::spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
{
    uint8_t status = 0;
    ATOMIC_BLOCK_START;
    _spi.beginTransaction();
    digitalWrite(_slaveSelectPin, LOW);
    status = _spi.transfer(reg & ~RH_SPI_WRITE_MASK); // Send the start address with the write mask off
    while (len--)
	*dest++ = _spi.transfer(0);
    digitalWrite(_slaveSelectPin, HIGH);
    _spi.endTransaction();
    ATOMIC_BLOCK_END;
    return status;
}

uint8_t RHSPIDriver::spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
    uint8_t status = 0;
    ATOMIC_BLOCK_START;
    _spi.beginTransaction();
    digitalWrite(_slaveSelectPin, LOW);
    status = _spi.transfer(reg | RH_SPI_WRITE_MASK); // Send the start address with the write mask on
    while (len--)
	_spi.transfer(*src++);
    digitalWrite(_slaveSelectPin, HIGH);
    _spi.endTransaction();
    ATOMIC_BLOCK_END;
    return status;
}

/* By Henrique Bruno - UFRJ Minerva Rockets*/
void RHSPIDriver::spiBurstWriteArrayOfPointersOf4Bytes(uint8_t reg, uint8_t** src, uint8_t number4BytesVariables)
{
    // uint8_t status = 0;
    ATOMIC_BLOCK_START;
    _spi.beginTransaction();
    digitalWrite(_slaveSelectPin, LOW);
    //status = below line was here
    _spi.transfer(reg | RH_SPI_WRITE_MASK); // Send the start address with the write mask on
    while (number4BytesVariables--)
    {
       _spi.transfer(**src);
       _spi.transfer(*(*src)+1);
       _spi.transfer(*(*src)+2);
       _spi.transfer(*(*src++)+3);
    }
    digitalWrite(_slaveSelectPin, HIGH);
    _spi.endTransaction();
    ATOMIC_BLOCK_END;
}

/* By Henrique Bruno - UFRJ Minerva Rockets*/
void RHSPIDriver::spiBurstWriteArrayOfPointersOfSmartSizes(uint8_t reg, uint8_t** src, uint8_t sizesArray[], uint8_t numberVariables)
{
    // uint8_t status = 0;
    ATOMIC_BLOCK_START;
    _spi.beginTransaction();
    digitalWrite(_slaveSelectPin, LOW);
    //status = below line was here
    _spi.transfer(reg | RH_SPI_WRITE_MASK); // Send the start address with the write mask on
    while (numberVariables--)
    {
        switch (*sizesArray) // Faster than a loop!
        {
            case (1):
                _spi.transfer(*(*src));
                break;
            case (2):
                _spi.transfer(*(*src));
                _spi.transfer(*(*src)+1);
                break;
            case (4):
                _spi.transfer(*(*src));
                _spi.transfer(*(*src)+1);
                _spi.transfer(*(*src)+2);
                _spi.transfer(*(*src)+3);
                break;
            case (8):
                _spi.transfer(*(*src));
                _spi.transfer(*(*src)+1);
                _spi.transfer(*(*src)+2);
                _spi.transfer(*(*src)+3);
                _spi.transfer(*(*src)+4);
                _spi.transfer(*(*src)+5);
                _spi.transfer(*(*src)+6);
                _spi.transfer(*(*src)+7);
                break;
            default: // Maybe will avoid random cosmic rays problems! (this isn't a proper error avoidance, time is always running out :P, but this is better than nothing)
                break;
        } // End of switch!
        src++;
        sizesArray++;
    }
    digitalWrite(_slaveSelectPin, HIGH);
    _spi.endTransaction();
    ATOMIC_BLOCK_END;
}

void RHSPIDriver::setSlaveSelectPin(uint8_t slaveSelectPin)
{
    _slaveSelectPin = slaveSelectPin;
}

void RHSPIDriver::spiUsingInterrupt(uint8_t interruptNumber)
{
    _spi.usingInterrupt(interruptNumber);
}
