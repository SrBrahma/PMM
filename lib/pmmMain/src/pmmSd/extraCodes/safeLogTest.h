#ifndef SAFE_LOG_h
#define SAFE_LOG_h

#include <Arduino.h>
#include <pmmSd/pmmSd.h>
#include <pmmSd/pmmSdSafeLog.h>


class SafeLogTest   // Being a class allows us to have functions inside, instead of global functions.
{

public:

    PmmSdSafeLog pmmSdSafeLog;
    SdFatSdio sdFat;
    
    SafeLogTest()
    {

        uint32_t  groupLength;
        bool      doQuit = false;

        PmmSd     pmmSd;

        

        uint32_t  bgnBlock, endBlock, currentBlock, currentPart, totalParts;

        Serial.println("SafeLogTest");
        Serial.println("Each file part will have the small size possible, according to the Cluster size");
        Serial.print  ("Group size is "); Serial.print(groupLength); Serial.println(" bytes.");




        while (!doQuit)
        {
            Serial.print("Part "); Serial.print(currentPart); Serial.print(" from a total of "); Serial.print(totalParts); Serial.println("Parts.");
            Serial.print("Relative Block ("); Serial.print(currentBlock - bgnBlock); Serial.print(") of Relative Block "); Serial.println(endBlock); Serial.println(".");
            printBlock(
        }

    }

    const unsigned MAX_BYTES_PER_LINE = 80;

    void printBlock(uint8_t blockContent[512])
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