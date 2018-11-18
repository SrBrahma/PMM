/* crc.h
 * Code for CRC8 and CRC16 calculations.
 * For the authors, sources, and additional informations, > READ the crc.cpp <
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */




#ifndef CRC_h
#define CRC_h



#include <stdint.h>


#define CRC8_DEFAULT_VALUE 0x00

#define CRC16_DEFAULT_VALUE 0xFFFF



uint8_t crc8(uint8_t data[], uint8_t length, uint8_t crc = CRC8_DEFAULT_VALUE);
uint8_t crc8SingleByte(uint8_t data, uint8_t crc = CRC8_DEFAULT_VALUE);

uint16_t crc16(uint8_t data[], uint16_t length, uint16_t crc = CRC16_DEFAULT_VALUE);
uint16_t crc16SingleByte(uint8_t data, uint16_t crc = CRC16_DEFAULT_VALUE);



#endif