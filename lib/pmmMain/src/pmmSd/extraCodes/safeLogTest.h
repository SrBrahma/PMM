/* pmmSdSafeLog.h
 *
 * Just a quick code to test the safeLog system. This isn't intended at the moment to look good.
 * 
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil
 * */
 

#ifndef SAFE_LOG_h
#define SAFE_LOG_h

#include <Arduino.h>
#include <pmmSd/pmmSd.h>
#include <pmmSd/pmmSdSafeLog.h>

#define MAX_BYTES_PER_LINE      80
#define GROUP_LENGTH            5
#define MAX_FILE_PARTS          2

#define DIR_BASE_NAME           "test"
#define DIR_SUFFIX              "sLog"

class SafeLogTest   // Being a class allows us to have functions inside, instead of global functions.
{

public:

    SafeLogTest()

    {

        PmmSd pmmSd;
        uint8_t blockContent[512];

        uint8_t groupData[GROUP_LENGTH];

        bool      doQuit = false;
        uint32_t  bgnBlock, endBlock, currentBlock, currentPart, totalParts;

        pmmSdAllocationStatusStructType statusStruct;
        statusStruct.groupLength = GROUP_LENGTH;

        char path[128];

        Serial.println("SafeLogTest");
        
        
        if (pmmSd.init(0)); // sessionId = 0;
        {
            Serial.println("Error at pmmSd.init(). Quitting this");
            delete this;
        }

        Serial.println("Initialized successfully.");

        PmmSdSafeLog pmmSdSafeLog(&pmmSd, 1); // 1 is the smallest size possible.

        Serial.println("Each file part will have the smallest size possible, according to the Cluster size.");
        Serial.print  ("Each file part is = "); Serial.print(pmmSd.getSdFatPtr()->blocksPerCluster()); Serial.println(" bytes");
        Serial.print  ("Group size is = "); Serial.print(statusStruct.groupLength); Serial.println(" bytes.");

        snprintf(path, 128, "%s-%s-%u", DIR_BASE_NAME, DIR_SUFFIX, GROUP_LENGTH);

        pmmSdSafeLog.write(groupData, path, &statusStruct);

        while (!doQuit)
        {
            Serial.print("Part "); Serial.print(currentPart); Serial.print(" from a total of "); Serial.print(totalParts); Serial.println("Parts.");
            Serial.print("Relative Block ("); Serial.print(currentBlock - bgnBlock); Serial.print(") of Relative Block "); Serial.println(endBlock); Serial.println(".");
            pmmSd.getCardPtr()->readBlock(currentBlock, blockContent);
            printBlock(blockContent);
            printControls();

            switch (waitUntilReadChar())
            {
                case 'e':   // erase actual file
                    break;

                case 'w':   // write a group with its backups
                    pmmSdSafeLog.write(groupData, path, &statusStruct);
                    break;

                case 't':   // write until all the file parts are written
                    while (statusStruct.nextFilePart < MAX_FILE_PARTS
                        pmmSdSafeLog.write(groupData, path, &statusStruct);
                    break;

                case 'i':   // increase current file part
                    break;

                case 'k':   // decrease current file part
                    break;

                case 'j':   // decrease current block
                    break;

                case 'l':   // increase current block
                    break;

                case 'q':   // quit
                    doQuit = 1;
                    break;
                
                case 'x':   // remove file and quit
                    
                    break;
            }

        }

    }


    char waitUntilReadChar()
    {
        while (!Serial.available())
            delay(10); // So we don't read the serial at 180mhz lol

        return Serial.read();
    }

    void printBlock(uint8_t blockContent[512])
    {
        for (unsigned actualByte = 0; actualByte < 512; actualByte++)
        {
            Serial.print(blockContent[actualByte], HEX);
            if (actualByte % MAX_BYTES_PER_LINE == 0)
                Serial.println();
        }
    }

    void printControls()
    {
        Serial.println("[e] erases the actual file; [w] writes a group (with backups); [t] writes until max file parts");
        Serial.println("[i/k] next/prev current file part; [j/l] next/prev current block; [q] to quit; [x] to remove file and quit");
    }

};


#undef MAX_BYTES_PER_LINE

#undef MAX_FILE_PARTS



#endif


/*
void printPossibleGroupSizesToFitEntirelly(unsigned clusterSize, unsigned fileParts)
{
    unsigned rFlagSize, mFlagSize, aFlagSize;
    rFlagSize = mFlagSize = aFlagSize = 1;
    
    totalBlockNumber = clusterSize * fileParts;
    totalSize = totalBlockNumber * 512;
    
    
    
    
}*/