#include <string.h>

#include "pmmDebug.h"
#include "pmmSd/alloc/alloc.h"
#include "pmmSd/alloc/safeLog.h"


// These are files which are:
// 1) Pre-allocated with a length of X KiB
//  The pre-allocation will also erase previous garbages. So, the blocks will be filled with zeroes or ones, deppending on the flash memory vendor.
// 2) Separated in parts. Each part, will have the length of the previous item.
// 3) Each write, have the same length -- maybe for now?
// 4)

// It won't use SdFatSdioEX as it uses multi-block writing, and by doing it, we lose the control of the safety system.

// By having the backup blocks always ahead of the current block instead of a fixed place for them, we distribute the SD

// Init parent class: https://stackoverflow.com/a/120916/10247962
PmmSdSafeLog::PmmSdSafeLog(SdFatSdio* sdFat)
    : PmmSdAllocation(sdFat)
{
    // These 3 exists as I an confused of which option to use. This must be improved later.
    mSdFat    = sdFat;
    mSdioCard = mSdFat->card();
}



// READ: The allocStatus must be initSafeLogStatusStruct() before.
// The maximum groupLength is 255, and the code below is written with that in mind, and assuming the block size is 512 bytes.
// On the future it can be improved. Not hard.

// https://stackoverflow.com/questions/31256206/c-memory-alignment
// readBlock() and writeBlock() uses a local array of 512 bytes, and a memcpy() if it is requested.
// When time is available for me, I will write a function in pmmSd for reading and writing without these slowers, and making sure the blockBuffer is aligned to 4.
int PmmSdSafeLog::write(uint8_t data[], char dirFullRelativePath[], PmmSdAllocStatus* allocStatus, uint8_t externalBlockBuffer[])
{
    // I didn't want to create this variable and keep changing it, and I didn't want to use (PMM_SD_BLOCK_SIZE - allocStatus->currentPositionInBlock),
    // so I will use a define, which certainly I will regret in the future, but now, is the best option I can think.
    // Edit: Actually, now I think it was a very nice decision.
    #define remainingBytesInThisBlock_macro (PMM_SD_BLOCK_SIZE - allocStatus->currentPositionInBlock)

    if (!data)                { advPrintf("No data given");        return 1; }
    if (!dirFullRelativePath) { advPrintf("No directory given");   return 2; }
    if (!allocStatus)         { advPrintf("No allocStatus given"); return 3; }

    unsigned dataBytesRemaining = allocStatus->groupLength;
    unsigned hadWrittenGroupHeader = false;

    uint8_t* blockBuffer = (externalBlockBuffer? externalBlockBuffer : mBlockBuffer);

// 1) Is this the first time running for this allocStatus?
    if (allocStatus->currentBlock == 0)
    {
        allocStatus->currentPositionInBlock = 0; // To make sure it starts reseted.
        // The function below will also change some struct member values. Read the corresponding function definition.
        if (allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, allocStatus))
        {
            advPrintf("Error at allocateFilePart(), at First Time!\n");
            return 1;
        }
    }


// 2) Is the current block full?
    //  The behavior of the (allocStatus->currentPositionInBlock > PMM_SD_BLOCK_SIZE) case is probably horrible. It normally won't happen.
    else if (allocStatus->currentPositionInBlock >= PMM_SD_BLOCK_SIZE)
    {
        if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, allocStatus))
        {
            advPrintf("Error at nextBlockAndAllocIfNeeded(), at Partial Final Data!\n");
            return 2;
        }
        allocStatus->currentPositionInBlock = 0;
    }


// 3) If there is something in the actual block, we must first copy it to add the new data.
    else if (allocStatus->currentPositionInBlock > 0)
    {
        if (!externalBlockBuffer)   // Only readBlock if no external buffer given.
            mSdioCard->readBlock(allocStatus->currentBlock, blockBuffer); // For some reason, this function returns an error, but works fine. help?

        // We have two possible cases:
        //    4) If the new data needs another block.              -- Partial Initial Data
        //    5) If the new data fits the current block, entirely. -- Entire Data / Partial Final Data


// 4) Now we will see if the data needs another block. If the conditional is true, we need another block!
        if ((allocStatus->groupLength + 2) > remainingBytesInThisBlock_macro) // +2 for header and footer
        {
            // 4.1) We first add the group header, which will always fit the current block (as the currentPositionInBlock goes from 0 to 511, if >=512,
            //  it was treated on 2).)
            blockBuffer[allocStatus->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;
            hadWrittenGroupHeader = true;


            // 4.2) Now we add the data to the current block, if there is available space.
            if (remainingBytesInThisBlock_macro) // Avoids useless calls of memcpy
            {
                memcpy(blockBuffer + allocStatus->currentPositionInBlock, data, remainingBytesInThisBlock_macro);
                dataBytesRemaining -= remainingBytesInThisBlock_macro;


                // 4.3) Write the Partial Initial Data to the SD.
                if(!mSdioCard->writeBlock(allocStatus->currentBlock, blockBuffer))
                {
                    advPrintf("Error at writeBlock(), at Partial Initial Data!\n");
                    return 1;
                }
            }

            allocStatus->currentPositionInBlock = 0;

            // 4.4) As we filled the current block, we need to move to the next one.
            if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, allocStatus))
            {
                advPrintf("Error at nextBlockAndAllocIfNeeded(), at Partial Initial Data!\n");
                return 1;
            }

        } // End of 4).
    } // End of 3).


// 5) Write the block that fits entirely the block / Write the Partial Final Data.
    // 5.1) Write the Written Flag if it is a new block.
    if (allocStatus->currentPositionInBlock == 0)
        blockBuffer[allocStatus->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN;

    // 5.2) Write the Group Begin Flag, if not done already at 3.1).
    if (!hadWrittenGroupHeader)
        blockBuffer[allocStatus->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;

    // 5.3) Write the Entire Data, or the Partial Final Data. We check if there is still dataBytesRemaining, as the 3)
    // may only needs to write the Group End Flag.
    if (dataBytesRemaining)
    {
        memcpy(blockBuffer + allocStatus->currentPositionInBlock, data + (allocStatus->groupLength - dataBytesRemaining), dataBytesRemaining);

        allocStatus->currentPositionInBlock += dataBytesRemaining;
        dataBytesRemaining = 0;
    }

    // 5.4) Write the Group End Flag.
    blockBuffer[allocStatus->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_END;

    // 5.5) Erase any previous garbage after the written data.
    if (remainingBytesInThisBlock_macro)
        memset(blockBuffer + allocStatus->currentPositionInBlock, 0, remainingBytesInThisBlock_macro);

    // 5.6) Write the Last Valid Block to the SD.
    if(!mSdioCard->writeBlock(allocStatus->currentBlock, blockBuffer))
    {
        advPrintf("Error at writeBlock(), at Last Valid Block!\n");
        return 1;
    }

// 6) Save the current variables of allocStatus, as they will change during the writting of the backup block, and we want to restore
    // these values after it.
    uint32_t tempCurrentBlock = allocStatus->currentBlock;
    uint32_t tempFreeBlocks   = allocStatus->freeBlocksAfterCurrent;
    uint32_t tempNextPart     = allocStatus->nextFilePart;

    // 6.1) We need two blocks for the Backup Block 0 and Backup Block 1. As in this system we first write in the Backup Block 1
    //  and only then on the Backup Block 0, and it's possible that the Backup Block 1 is on another file part,
    //  we will get the address of the Backup Block 0, write on the Backup Block 1, and then, on the Backup Block 0.
    //    It only happens when freeBlocksAfterCurrent < 2, but the procedure is almost the same if it isn't needed.

    // 6.2) Go to the next block, the Backup Block 0.
    if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, allocStatus))
    {
        advPrintf("Error at nextBlockAndAllocIfNeeded(), at Backup Block 0!\n");
        return 1;
    }

    // 6.3) Get the address of this block, the Backup Block 0, as the Backup Block 1 may be on another part.
    uint32_t backupBlock0Address = allocStatus->currentBlock;


    // 6.4) Go to the next block, the Backup Block 1.
    if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, allocStatus))
    {
        advPrintf("Error at nextBlockAndAllocIfNeeded(), at Backup Block 1!\n");
        return 1;
    }


    // 6.5) Write the Backup Block 1.
    if(!mSdioCard->writeBlock(allocStatus->currentBlock, blockBuffer))
    {
        advPrintf("Error at writeBlock(), at Backup Block 1!\n");
        return 1;
    }


    // 6.6) Write the Backup Block 0.
    if(!mSdioCard->writeBlock(backupBlock0Address, blockBuffer))
    {
        advPrintf("Error at writeBlock(), at Backup Block 0!\n");
        return 1;
    }

    // 6.1.7) Return the previous values
    allocStatus->currentBlock           = tempCurrentBlock;
    allocStatus->freeBlocksAfterCurrent = tempFreeBlocks  ;
    allocStatus->nextFilePart           = tempNextPart    ;

    return 0;
}



// Returns by reference the number of actual file parts.
// Returns 0 if no error.
int PmmSdSafeLog::getNumberFileParts(char dirFullRelativePath[], uint8_t* fileParts)
{
    return PmmSdAllocation::getNumberFileParts(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, fileParts);
}

int PmmSdSafeLog::getFileRange(char dirFullRelativePath[], uint8_t filePart, uint32_t *beginBlock, uint32_t *endBlock)
{
    char filename[PMM_SD_FILENAME_MAX_LENGTH];
    getFilePartName(filename, dirFullRelativePath, filePart, PMM_SD_SAFE_LOG_FILENAME_EXTENSION);
    return PmmSdAllocation::getFileRange(filename, beginBlock, endBlock);
}



const char* PmmSdSafeLog::getFilenameExtension()
{
    return PMM_SD_SAFE_LOG_FILENAME_EXTENSION;
}