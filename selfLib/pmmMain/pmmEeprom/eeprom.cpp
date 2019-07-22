#include <stdint.h>     // uint8_t
#include <EEPROM.h>

#include <byteSelection.h>

#include "pmmEeprom/eeprom.h"

// Writes 0x00 to all addresses.
void    PmmEeprom::clearAll()
{
    for (int16_t i = 0; i < EEPROM.length(); i++)
        EEPROM.write(i, 0);
}

uint8_t  PmmEeprom::getSession() { return EEPROM.read(mIndexes::session); }
int      PmmEeprom::getMagnetometerCalibrationX() { return get32b(mIndexes::magnCalX); }
int      PmmEeprom::getMagnetometerCalibrationY() { return get32b(mIndexes::magnCalY); }
int      PmmEeprom::getMagnetometerCalibrationZ() { return get32b(mIndexes::magnCalZ); }

void     PmmEeprom::setSession(uint8_t session) { EEPROM.write(mIndexes::session, session); }
void     PmmEeprom::setMagnetometerCalibrationX(int offset) { set32b(mIndexes::magnCalX, offset); }
void     PmmEeprom::setMagnetometerCalibrationY(int offset) { set32b(mIndexes::magnCalY, offset); }
void     PmmEeprom::setMagnetometerCalibrationZ(int offset) { set32b(mIndexes::magnCalZ, offset); }



uint32_t PmmEeprom::get32b(int startingIndex)  {
    uint32_t value = 0;
    value = join4Bytes(EEPROM.read(startingIndex),
                       EEPROM.read(startingIndex + 1),
                       EEPROM.read(startingIndex + 2),
                       EEPROM.read(startingIndex + 3));
    return value;
}

void     PmmEeprom::set32b(int startingIndex, uint32_t value)    {
    EEPROM.write(startingIndex    , LSB0(value));
    EEPROM.write(startingIndex + 1, LSB1(value));
    EEPROM.write(startingIndex + 2, LSB2(value));
    EEPROM.write(startingIndex + 3, LSB3(value));
}