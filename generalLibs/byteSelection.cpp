/* byteSelection.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <stdint.h>
#include "byteSelection.h"

inline uint8_t MSB0(uint16_t value){ return value >> 8;}
inline uint8_t MSB1(uint16_t value){ return value;}

inline uint8_t MSB0(uint32_t value){ return value >> 24;}
inline uint8_t MSB1(uint32_t value){ return value >> 16;}
inline uint8_t MSB2(uint32_t value){ return value >> 8;}
inline uint8_t MSB3(uint32_t value){ return value;}

inline uint8_t LSB0(uint16_t value){ return value;}
inline uint8_t LSB1(uint16_t value){ return value >> 8;}

inline uint8_t LSB0(uint32_t value){ return value;}
inline uint8_t LSB1(uint32_t value){ return value >> 8;}
inline uint8_t LSB2(uint32_t value){ return value >> 16;}
inline uint8_t LSB3(uint32_t value){ return value >> 24;}
