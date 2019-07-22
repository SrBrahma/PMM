#ifndef PMM_EEPROM_h
#define PMM_EEPROM_h

#include <stdint.h>     // uint8_t

class PmmEeprom
{
public:
    // Writes 0x00 to all addresses.
    void    clearAll(); 

    uint8_t getSession();
    int     getMagnetometerCalibrationX();
    int     getMagnetometerCalibrationY();
    int     getMagnetometerCalibrationZ();

    void    setSession(uint8_t session);
    void    setMagnetometerCalibrationX(int offset);
    void    setMagnetometerCalibrationY(int offset);
    void    setMagnetometerCalibrationZ(int offset);

private:
    uint32_t get32b(int startingIndex);
    void     set32b(int startingIndex, uint32_t value);

    enum mIndexes {session,
                   magnCalX,
                   magnCalY = magnCalX + 4,
                   magnCalZ = magnCalY + 4,
                  };
};


#endif