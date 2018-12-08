/* crc.cpp
 * Code for a CRC16 calculation.
 * The credits for the crc8 and the crc16 codes goes to the authors. Links to them are found on each function.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <stdint.h>
#include <crc.h>

// For all these functions, the crc argument is optional, as they are defaulted to a value in the crc.h.
// This argument exists so you can make sequential crc calculations even if the data isn't sequential, using the resulting CRC
//  as the argument for the next CRC calculation.
//
// Example:
// crc16("123456", 6)
// is the same as
// crc16("456", 3, crc("123", 3))





// =============               CRC-8               =======================

// CRC8 Function (ROM=39 / RAM=4 / Average => 196_Tcy / 24.5_us for 8MHz clock)
// CRC-8/MAXIM, as seen when comparing the results with this site https://crccalc.com/.
// Default value to crc argument is 0x00, as seen in crc.h.
//
// Found on this page: https://www.ccsinfo.com/forum/viewtopic.php?t=37015, by T. Scott Dattalo
// Changed a little bit to support arrays and uint8_t type.

uint8_t crc8(uint8_t data[], uint8_t length, uint8_t crc)
{
    uint8_t i;

    while (length--)
    {
        i = (*data++ ^ crc) & 0xff;
        crc = 0;

        if(i & 1)
            crc ^= 0x5e;
        if(i & 2)
            crc ^= 0xbc;
        if(i & 4)
            crc ^= 0x61;
        if(i & 8)
            crc ^= 0xc2;
        if(i & 0x10)
            crc ^= 0x9d;
        if(i & 0x20)
            crc ^= 0x23;
        if(i & 0x40)
            crc ^= 0x46;
        if(i & 0x80)
            crc ^= 0x8c;
    }

    return(crc);
}

uint8_t crc8SingleByte(uint8_t data, uint8_t crc)
{
    uint8_t i;

    i = (data ^ crc) & 0xff;
    crc = 0;

    if(i & 1)
        crc ^= 0x5e;
    if(i & 2)
        crc ^= 0xbc;
    if(i & 4)
        crc ^= 0x61;
    if(i & 8)
        crc ^= 0xc2;
    if(i & 0x10)
        crc ^= 0x9d;
    if(i & 0x20)
        crc ^= 0x23;
    if(i & 0x40)
        crc ^= 0x46;
    if(i & 0x80)
        crc ^= 0x8c;

    return(crc);
}





// ================         CRC16            =====================

// 
// CRC-16/CCITT-FALSE, as seen when comparing the results with this site https://crccalc.com/.
// Default value to crc argument is 0xFFFF, as seen in crc.h.
//
// Taken from https://stackoverflow.com/a/23726131
// Changed a little bit.
uint16_t crc16(uint8_t data[], uint16_t length, uint16_t crc)
{
    uint8_t x;

    while (length--)
    {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ (x << 12) ^ (x << 5 ^ x);
    }

    return crc;
}

// in crc.h uint16_t crc16SingleByte(uint8_t value, uint16_t crc = CRC16_DEFAULT_VALUE);
uint16_t crc16SingleByte(uint8_t data, uint16_t crc)
{
    uint8_t x;

    x = crc >> 8 ^ data;
    x ^= x >> 4;
    crc = (crc << 8) ^ (x << 12) ^ (x <<5 ^ x);

    return crc;
}

/*
I compared this very simple crc16 lib to the FastCRC16. This small function is somewhat better than the FastCRC16, according to this test.
Even if any parameter on FastCRC16 is wrong, the small function above is much smaller, and the time taken to calculate it is negligible.

The output from the code below is
This small crc16 function:  839ms for 1.000.000 iterations.
FastCRC16:                  851ms for 1.000.000 iterations.

Comparation done in Teensy 3.6. Yeah, it is fast!.


FastCRC16 CRC16;
int i = 1000000;
unsigned long tempMillis;

uint8_t buf[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

Serial.println("1) time for 1000 is = ");
tempMillis = millis();
while (i--)
    crc16(buf, 9);
    //Serial.print(6);
Serial.println(); Serial.println();
Serial.println(millis() - tempMillis);

Serial.println("\n\n2) time for 1000 is = ");
tempMillis = millis();

i = 1000000;
while (i--)
    CRC16.ccitt(buf, 9);
Serial.println();Serial.println();
Serial.println(millis() - tempMillis);
Serial.println("\n\nYA!");
delay(30000);
*/
