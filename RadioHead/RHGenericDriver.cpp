// RHGenericDriver.cpp
//
// Copyright (C) 2014 Mike McCauley
// $Id: RHGenericDriver.cpp,v 1.23 2018/02/11 23:57:18 mikem Exp mikem $

#include <RHGenericDriver.h>

RHGenericDriver::RHGenericDriver()
{
}

bool RHGenericDriver::init()
{
    mThisAddress = RH_THIS_SYSTEM_ADDRESS;
    _txHeaderTo = RH_BROADCAST_ADDRESS;
    _txHeaderFrom = RH_BROADCAST_ADDRESS;
    mInvalidReceivedPacketsCounter = 0,
    mMode = RH_MODE_IS_INITIALIZING;
    mSuccessfulReceivedPacketsCounter = 0;
    _txGood = 0;
    _cad_timeout = 0;
    return true;
}

// Blocks until a valid message is received
void RHGenericDriver::waitAvailable()
{
    while (!available())
        YIELD;
}

// Blocks until a valid message is received or timeout expires
// Return true if there is a message available
// Works correctly even on millis() rollover
bool RHGenericDriver::waitAvailableTimeout(uint16_t timeout)
{
    unsigned long starttime = millis();
    while ((millis() - starttime) < timeout)
    {
        if (available())
        {
            return true;
        }
        YIELD;
    }
    return false;
}

// By Henrique Bruno, Minerva Rockets - UFRJ
bool RHGenericDriver::isAnyPacketBeingSentRHGenericDriver()
{
    if (mMode == RH_MODE_IS_TRANSMITTING)
        return true; // Yes! There is a packet being sent!

    return false;    // No! No packet being sent!
}

bool RHGenericDriver::waitPacketSent()
{
    while (mMode == RH_MODE_IS_TRANSMITTING)
        YIELD; // Wait for any previous transmit to finish
    return true;
}

bool RHGenericDriver::waitPacketSent(uint16_t timeout)
{
    unsigned long starttime = millis();
    while ((millis() - starttime) < timeout)
    {
        if (mMode != RH_MODE_IS_TRANSMITTING) // Any previous transmit finished?
           return true;
        YIELD;
    }
    return false;
}

// Wait until no channel activity detected or timeout
bool RHGenericDriver::waitCAD()
{
    if (!_cad_timeout)
        return true;

    // Wait for any channel activity to finish or timeout
    // Sophisticated DCF function...
    // DCF : BackoffTime = random() x aSlotTime
    // 100 - 1000 ms
    // 10 sec timeout
    unsigned long t = millis();
    while (isChannelActive())
    {
        if (millis() - t > _cad_timeout)
            return false;

        #if (RH_PLATFORM == RH_PLATFORM_STM32) // stdlib on STMF103 gets confused if random is redefined
            delay(_random(1, 10) * 100);
        #else
            delay(random(1, 10) * 100); // Should these values be configurable? Macros?
        #endif
    }

    return true;
}

// subclasses are expected to override if CAD is available for that radio
bool RHGenericDriver::isChannelActive()
{
    return false;
}

void RHGenericDriver::setPromiscuous(bool promiscuous)
{
    mPromiscuousMode = promiscuous;
}

void RHGenericDriver::setThisAddress(uint8_t address)
{
    mThisAddress = address;
}

void RHGenericDriver::setHeaderTo(uint8_t to)
{
    _txHeaderTo = to;
}

void RHGenericDriver::setHeaderFrom(uint8_t from)
{
    _txHeaderFrom = from;
}

int16_t RHGenericDriver::getLastRssi()
{
    return mLastRssi;
}

RHGenericDriver::RHMode  RHGenericDriver::getMode()
{
    return mMode;
}

void  RHGenericDriver::setMode(RHMode mode)
{
    mMode = mode;
}

bool  RHGenericDriver::sleep()
{
    return false;
}

// Diagnostic help
void RHGenericDriver::printBuffer(const char* prompt, const uint8_t* buf, uint8_t len)
{
#ifdef RH_HAVE_SERIAL
    Serial.println(prompt);
    uint8_t i;
    for (i = 0; i < len; i++)
    {
	if (i % 16 == 15)
	    Serial.println(buf[i], HEX);
	else
	{
	    Serial.print(buf[i], HEX);
	    Serial.print(' ');
	}
    }
    Serial.println("");
#endif
}

uint16_t RHGenericDriver::rxBad()
{
    return mInvalidReceivedPacketsCounter;
}

uint16_t RHGenericDriver::rxGood()
{
    return mSuccessfulReceivedPacketsCounter;
}

uint16_t RHGenericDriver::txGood()
{
    return _txGood;
}

void RHGenericDriver::setCADTimeout(unsigned long cad_timeout)
{
    _cad_timeout = cad_timeout;
}

#if (RH_PLATFORM == RH_PLATFORM_ARDUINO) && defined(RH_PLATFORM_ATTINY)
// Tinycore does not have __cxa_pure_virtual, so without this we
// get linking complaints from the default code generated for pure virtual functions
extern "C" void __cxa_pure_virtual()
{
    while (1);
}
#endif
