/* crc16.h
 * Code for a CRC16 calculation.
 * The credits for it goes to the author, found on the Stack Overflow link in crc16.cpp.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef CRC16_h
#define CRC16_h
/* See the crc16.cpp for some texts. */
#define CRC16_DEFAULT_VALUE 0xFFFF
#include <stdint.h>

uint16_t crc16(uint8_t* data_p, uint16_t length, uint16_t crc = CRC16_DEFAULT_VALUE);

#endif
