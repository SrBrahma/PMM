/* crc16.cpp
 * Code for a CRC16 calculation.
 * The credits for it goes to the author, found on the Stack Overflow link below.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <stdint.h>
#include <crc16.h>

// Taken from https://stackoverflow.com/a/23726131 with a few changes.
// crc has the default value of 0xFFFF, declared on crc16.h.
// changing the crc value is useful for doing multiple crc's in sequence, like
// crc("123456", 6) is the same as
// crc("456", 3, crc("123", 3))

// uint16_t crc16(uint8_t* data_p, uint16_t length, uint16_t crc = 0xFFFF);
uint16_t crc16(uint8_t* data_p, uint16_t length, uint16_t crc)
{
    uint8_t x;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ (x << 12) ^ (x <<5 ^ x);
    }
    return crc;
}

/*
I compared this very simple crc16 lib to the FastCRC16. This small function is somewhat better than the FastCRC16, according to this test.
Even if any parameter on FastCRC16 is wrong, the small function above is much smaller, and the time taken to calculate it is negligible.

The output from the code below is
This small crc16 function:  839ms for 1.000.000 iterations.
FastCRC16:                  851ms for 1.000.000 iterations.

Comparation done in Teensy 3.6. Yeah, it is fast as #@&*!.


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
