#ifndef PMM_EEPROM_h
#define PMM_EEPROM_h

#include <stdint.h> // For uint8_t type

#define PMM_EEPROM_VERSION_STRING                       "PMM EEPROM Version 1.0"   // This is really intended to be big,
// to avoid the eventual exact match of a previous EEPROM content, so we can be "sure" that the stored content belongs or not to this code.
#define PMM_EEPROM_VERSION_STRING_LENGTH_W_NULL_CHAR    sizeof(PMM_EEPROM_VERSION_STRING) // https://stackoverflow.com/a/4003425/10247962

// These indexes below are explicit for an easier eventual reverse-engineering of the Eeprom.
#define PMM_EEPROM_INDEX_VERSION_STRING_START           0
#define PMM_EEPROM_INDEX_VERSION_STRING_END             22 // Version String length with '\0' is 23. As it started on 0, the end is at position 22.

#define PMM_EEPROM_INDEX_SESSION_ID                     23
#define PMM_EEPROM_DEFAULT_VALUE_SESSION_ID             0

class PmmEeprom
{
public:
    int init();
    uint8_t getSessionId();
    void setSessionId(uint8_t value);

private:
    void writeDefaultValues();
};

#endif