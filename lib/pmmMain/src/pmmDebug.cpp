#include <Arduino.h>    // For Serial
#include <stdint.h>     // for uintX_t
#include <string.h>     // For snprintf
#include "pmmDebug.h"

void printArrayHex(uint8_t arrayToPrint[], unsigned arrayLength)
{
    char buffer[4];

    for (unsigned actualByte = 0; actualByte < arrayLength; actualByte++)
    {
        snprintf(buffer, 4, "%02X ", arrayToPrint[actualByte]);
        Serial.print(buffer);
        if ((actualByte + 1) % PMM_DEBUG_PRINT_HEX_MAX_BYTES_PER_LINE == 0)
            Serial.println();
    }
    Serial.println();
}