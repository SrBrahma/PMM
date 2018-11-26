/* byteSelection.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef BYTE_SELECTION_h
#define BYTE_SELECTION_h

#include <stdint.h>

// As they are inline, the definition must be together with the declaration
inline uint8_t MSB0(uint16_t value) { return value >> 8;  }
inline uint8_t MSB1(uint16_t value) { return value;       }

inline uint8_t MSB0(uint32_t value) { return value >> 24; }
inline uint8_t MSB1(uint32_t value) { return value >> 16; }
inline uint8_t MSB2(uint32_t value) { return value >> 8;  }
inline uint8_t MSB3(uint32_t value) { return value;       }

inline uint8_t LSB0(uint16_t value) { return value;       }
inline uint8_t LSB1(uint16_t value) { return value >> 8;  }

inline uint8_t LSB0(uint32_t value) { return value;       }
inline uint8_t LSB1(uint32_t value) { return value >> 8;  }
inline uint8_t LSB2(uint32_t value) { return value >> 16; }
inline uint8_t LSB3(uint32_t value) { return value >> 24; }



inline bool getBit(unsigned value, unsigned bitPositionLsb)
{
    return (value >> bitPositionLsb) & 1;
}

inline void setBit(uint8_t value, unsigned bitPositionLsb, bool bitValue)
{
    if (bitValue)
        value |=  (1 << bitPositionLsb);
    else
        value &= ~(1 << bitPositionLsb);
}

inline void setBit(uint16_t value, unsigned bitPositionLsb, bool bitValue)
{
    if (bitValue)
        value |=  (1 << bitPositionLsb);
    else
        value &= ~(1 << bitPositionLsb);
}

inline void setBit(uint32_t value, unsigned bitPositionLsb, bool bitValue)
{
    if (bitValue)
        value |=  (1 << bitPositionLsb);
    else
        value &= ~(1 << bitPositionLsb);
}



#endif
