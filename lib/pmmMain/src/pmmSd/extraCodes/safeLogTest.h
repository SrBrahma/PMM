/* mPmmSdSafeLog.h
 *
 * Just a quick code to test the SafeLog system. This isn't intended at the moment to look good.
 * 
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil
 * */
 

#ifndef SAFE_LOG_h
#define SAFE_LOG_h

#include <Arduino.h>
#include "pmmSd/pmmSd.h"
#include "pmmSd/pmmSdAllocation.h"
#include "pmmSd/pmmSdSafeLog.h"

#define MAX_BYTES_PER_LINE      49
#define GROUP_LENGTH            5
#define MAX_FILE_PARTS          2

#define DIR_BASE_NAME           "test"
#define DIR_SUFFIX              "sLog"

class SafeLogTest   // Being a class allows us to have functions inside, instead of global functions.
{

public:

    SafeLogTest()
    : mPmmSdSafeLog(&mPmmSd, 64) // 1 will create the file with the smallest cluster size possible.
    {}



    int main()
    {
        
        Serial.println("SafeLogTest, intializing...");
        
        if (init())
        {
            Serial.println("Fail at init(), quitting.\n");
            return 1;
        }

        if (reset())
        {
            Serial.println("Fail at reset(), quitting.\n");
            return 2;
        }
        Serial.println(mStatusStruct.groupLength);

        Serial.println("Initialized successfully.");

        Serial.println("Each file part will have the smallest size possible, according to the Cluster size.");
        Serial.print  ("Each file part is = "); Serial.print(mPmmSd.getSdFatPtr()->blocksPerCluster()); Serial.println(" blocks (512 bytes each).");
        Serial.print  ("Group size is = "); Serial.print(mStatusStruct.groupLength); Serial.println(" bytes, without the 2 bytes of Begin and End flags.");
        Serial.println();

        while (!mDoQuit)
        {
            mPmmSdSafeLog.getNumberFileParts(mDirPath, &mTotalParts);

            mPmmSdSafeLog.getFileRange(mDirPath, mStatusStruct.nextFilePart - 1, &mBeginBlockWorking, &mEndBlockWorking);
            Serial.println("Working:");
            Serial.print("Part  ["); Serial.print(mStatusStruct.nextFilePart - 1); Serial.print("] of ["); Serial.print(mTotalParts - 1); Serial.println("].");
            Serial.print("Block ["); Serial.print(mStatusStruct.currentBlock - mBeginBlockWorking); Serial.print("] of ["); Serial.print(mEndBlockWorking - mBeginBlockWorking); Serial.println("].");
            

            Serial.println("Showing:");
            Serial.print("Part  ["); Serial.print(mCurrentPartShowing); Serial.print("] of ["); Serial.print(mTotalParts - 1); Serial.println("].");
            Serial.print("Block ["); Serial.print(mCurrentBlockShowing - mBeginBlockShowing); Serial.print("] of ["); Serial.print(mEndBlockShowing - mBeginBlockShowing); Serial.println("].");
            
            mPmmSd.getCardPtr()->readBlock(mCurrentBlockShowing, mBlockContent);
            printBlock(mBlockContent);

            printControls();
            switch (waitUntilReadChar())
            {
                case 'r':   // reset the program
                    reset();
                    break;

                case 'w':   // write a group with its backups
                    writeGroup();
                    break;

                case 't':   // write until actual file is filled
                    while (mStatusStruct.freeBlocksAfterCurrent > 2)
                        writeGroup();
                    break;

                case 'i':   // increase current file part
                    (mCurrentPartShowing >= mTotalParts - 1) ? (mCurrentPartShowing = 0) : mCurrentPartShowing++;
                    mPmmSdSafeLog.getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
                    break;

                case 'k':   // decrease current file part
                    (mCurrentPartShowing == 0) ? (mCurrentPartShowing = mTotalParts - 1) : mCurrentPartShowing--;
                    mPmmSdSafeLog.getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
                    break;

                case 'l':   // increase current block
                    // If this is the last block of the part
                    if (mCurrentBlockShowing >= mEndBlockShowing)
                    {
                        (mCurrentPartShowing >= mTotalParts - 1) ? (mCurrentPartShowing = 0) : mCurrentPartShowing++;
                        mPmmSdSafeLog.getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
                        mCurrentBlockShowing = mBeginBlockShowing;
                    }
                    else
                        mCurrentBlockShowing++;
                    break;

                case 'j':   // decrease current block
                    // If this is the first block of the part
                    if (mCurrentBlockShowing <= mBeginBlockShowing)
                    {
                        (mCurrentPartShowing == 0) ? (mCurrentPartShowing = mTotalParts - 1) : mCurrentPartShowing--;
                        mPmmSdSafeLog.getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
                        mCurrentBlockShowing = mEndBlockShowing;
                    }
                    else
                        mCurrentBlockShowing--;
                    break;

                case 'q':   // quit
                    mDoQuit = 1;
                    break;
                
                case 'x':   // remove file and quit
                    mPmmSd.removeDirRecursively(mDirPath);
                    mDoQuit = 1;
                    break;
            } // End of Switch

        } // End of while (!mDoQuit)
        return 0;
    } // End of constructor





private:
    PmmSd mPmmSd;

    PmmSdSafeLog mPmmSdSafeLog;

    pmmSdAllocationStatusStructType mStatusStruct;

    uint32_t mBeginBlockShowing, mEndBlockShowing, mCurrentBlockShowing;
    uint32_t mBeginBlockWorking, mEndBlockWorking;

    uint8_t  mCurrentPartShowing, mTotalParts;

    uint8_t mBlockContent[512];

    uint8_t mGroupData[GROUP_LENGTH];

 
    bool mDoQuit;
    char mDirPath[128];


    int init()
    {
        snprintf(mDirPath, 128, "%s-%s-%u", DIR_BASE_NAME, DIR_SUFFIX, GROUP_LENGTH);
        mDoQuit = false;
        
        return mPmmSd.init(0); // sessionId = 0;
    }

    int reset()
    {
        for (unsigned i = 0; i < GROUP_LENGTH; i++) { mGroupData[i] = i; }   // Give the initial values of the group data, just for a better demonstration.
        
        mPmmSdSafeLog.initSafeLogStatusStruct(&mStatusStruct, GROUP_LENGTH);
        Serial.print("2groupLenght is "); Serial.println(mStatusStruct.groupLength);

        // remove previous contents from the dir, if exists
        if (mPmmSd.getSdFatPtr()->exists(mDirPath))
        {
            //if (mPmmSd.removeDirRecursively(mDirPath))
                //return 1;
        }


        // Write the first group, so we can show something.
        if (mPmmSdSafeLog.write(mGroupData, mDirPath, &mStatusStruct))
            return 2;
        Serial.print("3groupLenght is "); Serial.println(mStatusStruct.groupLength);
        mBeginBlockShowing = mStatusStruct.currentBlock;
        mEndBlockShowing = mStatusStruct.currentBlock + mStatusStruct.freeBlocksAfterCurrent;
        
        mCurrentBlockShowing = mBeginBlockShowing;
        mCurrentPartShowing = 0;

        return 0;
    }

    int writeGroup()
    {
        if (mPmmSdSafeLog.write(mGroupData, mDirPath, &mStatusStruct))
            return 1;
        for (unsigned i = 0; i < GROUP_LENGTH; i++)
        {
            mGroupData[i]++;
        }
        return 0;
    }

    char waitUntilReadChar()
    {
        while (!Serial.available())
            delay(10); // So we don't read the serial at 180mhz lol

        return Serial.read();
    }

    void printBlock(uint8_t mBlockContent[512])
    {
        char buffer[4];

        for (unsigned actualByte = 0; actualByte < 512; actualByte++)
        {
            snprintf(buffer, 4, "%02X ", mBlockContent[actualByte]);
            Serial.print(buffer);
            if ((actualByte + 1) % MAX_BYTES_PER_LINE == 0)
                Serial.println();
        }
        Serial.println();
    }

    void printControls()
    {
        Serial.println("[i/k] next/prev current file part; [j/l] next/prev current block; [w] writes a group (with backups);");
        Serial.println("[t] writes until actual file is filled; [r] reset the program; [q] to quit; [x] to remove file and quit");
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