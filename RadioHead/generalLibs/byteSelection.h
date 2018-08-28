#ifndef BYTE_SELECTION_h
#define BYTE_SELECTION_h

#include <stdint.h>

uint8_t MSB0(uint16_t value);
uint8_t MSB1(uint16_t value);

uint8_t MSB0(uint32_t value);
uint8_t MSB1(uint32_t value);
uint8_t MSB2(uint32_t value);
uint8_t MSB3(uint32_t value);

uint8_t LSB0(uint16_t value);
uint8_t LSB1(uint16_t value);

uint8_t LSB0(uint32_t value);
uint8_t LSB1(uint32_t value);
uint8_t LSB2(uint32_t value);
uint8_t LSB3(uint32_t value);

#endif
