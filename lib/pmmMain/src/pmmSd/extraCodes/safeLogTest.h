/* pmmSdSafeLog.h
 *
 * Just a quick code to test the SafeLog system. This isn't intended at the moment to look good.
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

        PmmSdSafeLog pmmSdSafeLog(&pmmSd, 1); // 1 will create the file with the smallest cluster size possible.

        pmmSdAllocationStatusStructType statusStruct;

        statusStruct.groupLength = GROUP_LENGTH;


        uint8_t blockContent[512];

        uint8_t groupData[GROUP_LENGTH];
        for (unsigned i; i < GROUP_LENGTH; i++) { groupData[i] = i; }   // Give the initial values of the group data, just for a better demonstration.

        uint32_t bgnBlock, endBlock, currentBlockShowing, currentPartShowing, totalParts;

        bool doQuit = false;
        char path[128];



        Serial.println("SafeLogTest, intializing...");
        
        if (pmmSd.init(0)); // sessionId = 0;
        {
            Serial.println("Error at pmmSd.init(). Quitting this.\n");
            delete this;
        }

        Serial.println("Initialized successfully.");

        Serial.println("Each file part will have the smallest size possible, according to the Cluster size.");
        Serial.print  ("Each file part is = "); Serial.print(pmmSd.getSdFatPtr()->blocksPerCluster()); Serial.println(" bytes");
        Serial.print  ("Group size is = "); Serial.print(statusStruct.groupLength); Serial.println(" bytes, without the 2 bytes of Begin and End flags.");

        // directory name
        snprintf(path, 128, "%s-%s-%u", DIR_BASE_NAME, DIR_SUFFIX, GROUP_LENGTH);

        // Write the first group, so we can show something.
        pmmSdSafeLog.write(groupData, path, &statusStruct);

        while (!doQuit)
        {
            pmmSdSafeLog.getNumberFileParts(path, pmmSdSafeLog.getFilenameExtension, &totalParts);
            Serial.print("Part "); Serial.print(currentPartShowing); Serial.print(" from a total of "); Serial.print(totalParts); Serial.println("Parts.");

            Serial.print("Relative Block ("); Serial.print(currentBlockShowing - bgnBlock); Serial.print(") of Relative Block "); Serial.println(endBlock); Serial.println(".");
            pmmSd.getCardPtr()->readBlock(currentBlockShowing, blockContent);
            printBlock(blockContent);
            printControls();

            switch (waitUntilReadChar())
            {
                case 'e':   // erase actual file
                    break;

                case 'w':   // write a group with its backups
                    writeGroup(&pmmSdSafeLog, groupData, path, &statusStruct);
                    break;

                case 't':   // write until actual file is filled
                    while (statusStruct.freeBlocksAfterCurrent > 2)
                        writeGroup(&pmmSdSafeLog, groupData, path, &statusStruct);
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

    int writeGroup(PmmSdSafeLog* pmmSdSafeLog, uint8_t groupData[], char path[], pmmSdAllocationStatusStructType *statusStruct)
    {
        pmmSdSafeLog->write(groupData, path, statusStruct);
        for (unsigned i = 0; i < GROUP_LENGTH; i++)
        {
            groupData[i]++;
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
        Serial.println("[e] erases the actual file; [w] writes a group (with backups); [t] writes until actual file is filled");
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