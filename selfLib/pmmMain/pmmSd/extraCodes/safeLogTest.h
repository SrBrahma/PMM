/* mPmmSdSafeLog->h
 *
 * Just a quick code to test the SafeLog system. This isn't intended at the moment to look good.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil
 * */


#ifndef SAFE_LOG_h
#define SAFE_LOG_h

#include <Arduino.h>
#include "pmmSd/sd.h"
#include "pmmDebug.h"

#define GROUP_LENGTH            150
#define MAX_FILE_PARTS          2

#define DIR_BASE_NAME           "test"
#define DIR_SUFFIX              "sLog"

class SafeLogTest   // Being a class allows us to have functions inside, instead of global functions.
{

public:

    SafeLogTest()
        : mStatusStruct(GROUP_LENGTH, 32)
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

        Serial.println("Initialized successfully.");

        Serial.println("Each file part will have the smallest size possible, according to the Cluster size.");
        Serial.print  ("Each file part is = "); Serial.print(mPmmSd.getSdFatPtr()->blocksPerCluster()); Serial.println(" blocks (512 bytes each).");
        Serial.print  ("Group size is = "); Serial.print(mStatusStruct.groupLength); Serial.println(" bytes, without the 2 bytes of Begin and End flags.");
        Serial.println();

        while (!mDoQuit)
        {
            mPmmSdSafeLog->getNumberFileParts(mDirPath, &mTotalParts);

            mPmmSdSafeLog->getFileRange(mDirPath, mStatusStruct.nextFilePart - 1, &mBeginBlockWorking, &mEndBlockWorking);
            Serial.print("Working: ");
            Serial.print("Part  ["); Serial.print(mStatusStruct.nextFilePart - 1); Serial.print("] of ["); Serial.print(mTotalParts - 1); Serial.print("]; ");
            Serial.print("Block ["); Serial.print(mStatusStruct.currentBlock - mBeginBlockWorking); Serial.print("] of ["); Serial.print(mEndBlockWorking - mBeginBlockWorking); Serial.println("].");


            Serial.print("Showing: ");
            Serial.print("Part  ["); Serial.print(mCurrentPartShowing); Serial.print("] of ["); Serial.print(mTotalParts - 1); Serial.print("]; ");
            Serial.print("Block ["); Serial.print(mCurrentBlockShowing - mBeginBlockShowing); Serial.print("] of ["); Serial.print(mEndBlockShowing - mBeginBlockShowing); Serial.println("].");

            mPmmSd.getCardPtr()->readBlock(mCurrentBlockShowing, mBlockContent);
            printArrayHex(mBlockContent, 512);

            printControls();
            Serial.println();

            switch (waitUntilReadChar())
            {
                case 'r':   // reset the program
                    reset();
                    break;

                case 'w':   // write a group with its backups
                    writeGroup();
                    break;

                case 't':   // write until actual file is filled
                {
                    uint32_t mil = millis(), writeGroupCalls = 0;
                    while (mStatusStruct.freeBlocksAfterCurrent)
                    {
                        writeGroup();
                        writeGroupCalls++;
                    }

                    Serial.print("Time passed / Write group calls = "); Serial.print(millis() - mil); Serial.print(" / "); Serial.print(writeGroupCalls);
                    Serial.print(" = "); Serial.print((millis() - mil) / float(writeGroupCalls)); Serial.print(" ms per writeGroup call.");
                    break;
                }

                case 'i':   // increase current file part
                    if (mTotalParts > 1)
                    {
                        (mCurrentPartShowing >= mTotalParts - 1) ? (mCurrentPartShowing = 0) : mCurrentPartShowing++;
                        mPmmSdSafeLog->getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
                        mCurrentBlockShowing = mBeginBlockShowing;
                    }
                    break;

                case 'k':   // decrease current file part
                    if (mTotalParts > 1)
                    {
                        (mCurrentPartShowing == 0) ? (mCurrentPartShowing = mTotalParts - 1) : mCurrentPartShowing--;
                        mPmmSdSafeLog->getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
                        mCurrentBlockShowing = mBeginBlockShowing;
                    }
                    break;

                case 'l':   // increase current block
                    // If this is the last block of the part
                    if (mCurrentBlockShowing >= mEndBlockShowing)
                    {
                        (mCurrentPartShowing >= mTotalParts - 1) ? (mCurrentPartShowing = 0) : mCurrentPartShowing++;
                        mPmmSdSafeLog->getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
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
                        mPmmSdSafeLog->getFileRange(mDirPath, mCurrentPartShowing, &mBeginBlockShowing, &mEndBlockShowing);
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
    uint8_t mBlockContent[512];

    PmmSd mPmmSd;

    PmmSdSafeLog* mPmmSdSafeLog;

    PmmSdAllocStatus mStatusStruct;

    uint32_t mBeginBlockShowing, mEndBlockShowing, mCurrentBlockShowing;
    uint32_t mBeginBlockWorking, mEndBlockWorking;

    uint8_t  mCurrentPartShowing, mTotalParts;



    uint8_t mGroupData[GROUP_LENGTH];


    bool mDoQuit;
    char mDirPath[128];


    int init()
    {
        snprintf(mDirPath, 128, "%s-%s-%u", DIR_BASE_NAME, DIR_SUFFIX, GROUP_LENGTH);
        mDoQuit = false;
        int initValue = mPmmSd.init(0);
        mPmmSdSafeLog = mPmmSd.getSafeLog();
        return initValue; // sessionId = 0;
    }


    int reset()
    {
        for (unsigned i = 0; i < GROUP_LENGTH; i++) { mGroupData[i] = i; }   // Give the initial values of the group data, just for a better demonstration.

        mStatusStruct.reset(GROUP_LENGTH, 32);

        // remove previous contents from the dir, if exists
        if (mPmmSd.getSdFatPtr()->exists(mDirPath))
        {
            mPmmSd.removeDirRecursively(mDirPath);
        }

        // Write the first group, so we can show something.
        if (writeGroup())
            return 2;

        mBeginBlockShowing   = mStatusStruct.currentBlock;
        mEndBlockShowing     = mStatusStruct.currentBlock + mStatusStruct.freeBlocksAfterCurrent;

        mCurrentBlockShowing = mBeginBlockShowing;
        mCurrentPartShowing  = 0;

        return 0;
    }


    int writeGroup()
    {
        if (mPmmSdSafeLog->write(mGroupData, mDirPath, &mStatusStruct))
            return 1;

        uint8_t value = mGroupData[GROUP_LENGTH - 1];

        for (unsigned i = 0; i < GROUP_LENGTH; i++)
            mGroupData[i] = value + i;

        return 0;
    }


    char waitUntilReadChar()
    {
        while (!Serial.available())
            delay(10); // So we don't read the serial at 180mhz lol

        return Serial.read();
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