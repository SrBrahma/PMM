#ifndef SAFE_LOG_h
#define SAFE_LOG_h

class SafeLogTest
{
public:

    const unsigned MAX_BYTES_PER_LINE = 80;

    printBlock(uint8_t blockContent[512])
    {
        for (unsigned actualByte = 0; actualByte < 512; actualByte++)
        {
            Serial.print(blockContent[actualByte], HEX);
            if (actualByte % MAX_BYTES_PER_LINE == 0)
                Serial.println();
        }
    }

};
#endif