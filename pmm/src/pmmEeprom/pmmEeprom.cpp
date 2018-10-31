#include <stdint.h> // For uint8_t type
#include <EEPROM.h>

#include "pmmEeprom/pmmEeprom.h"

int PmmEeprom::init()
{
    // 1) Compare the PMM_EEPROM_VERSION_STRING with some addresses of the EEPROM, to check if this is the first time
    // this code is running (if the version changed, it also counts!)
    uint8_t intendedVersionString[] = PMM_EEPROM_VERSION_STRING;

    unsigned counter;

    // 1.1) Scans all the characters where the string of the Version should be located.
    for (counter = 0; counter < PMM_EEPROM_VERSION_STRING_LENGTH_W_NULL_CHAR; counter++)
    {
        // 1.2) If what the current content is wrong, well, write the default values!
        if (EEPROM.read(counter + PMM_EEPROM_INDEX_VERSION_STRING_START) != intendedVersionString[counter])
            writeDefaultValues();
    }
    return 0;
}

uint8_t PmmEeprom::getSessionId()
{
    return EEPROM.read(PMM_EEPROM_INDEX_SESSION_ID);
}

void PmmEeprom::setSessionId(uint8_t value)
{
    EEPROM.write(PMM_EEPROM_INDEX_SESSION_ID, value);
}


void PmmEeprom::writeDefaultValues()
{
    uint8_t intendedVersionString[] = PMM_EEPROM_VERSION_STRING;
    unsigned counter;

    // The version string is written after everything. Maybe this will prevent false-positives of a successfully writeDefaultValues()?

    // 1) Write the default value of the Session Id.
    EEPROM.write(PMM_EEPROM_INDEX_SESSION_ID, PMM_EEPROM_DEFAULT_VALUE_SESSION_ID);

    // 2) Write the version string.
    for (counter = 0; counter < PMM_EEPROM_VERSION_STRING_LENGTH_W_NULL_CHAR; counter++)
        EEPROM.write(PMM_EEPROM_INDEX_VERSION_STRING_START + counter, intendedVersionString[counter]);

}