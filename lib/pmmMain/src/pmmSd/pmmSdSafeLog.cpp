#include <string.h>
#include "pmmConsts.h"

#include "pmmSd/pmmSd.h"
#include "pmmSd/pmmSdAllocation.h"
#include "pmmSd/pmmSdSafeLog.h"

// These are files which are:
// 1) Pre-allocated with a length of X KiB
//  The pre-allocation will also erase previous garbages. So, the blocks will be filled with zeroes or ones, deppending on the flash memory vendor.
// 2) Separated in parts. Each part, will have the length of the previous item.
// 3) Each write, have the same length -- maybe for now?
// 4)

// It won't use SdFatSdioEX as it uses multi-block writing, and by doing it, we lose the control of the safety system.

// By having the backup blocks always ahead of the current block instead of a fixed place for them, we distribute the SD


PmmSdSafeLog::PmmSdSafeLog(PmmSd* pmmSd, unsigned defaulBlocksAllocationPerPart)
    : PmmSdAllocation(pmmSd->getSdFatPtr())
{
    // These 3 exists as I an confused of which option to use. This must be improved later.
    mPmmSd = pmmSd;
    mSdFat = pmmSd->getSdFatPtr();
    mSdioCard = pmmSd->getCardPtr();
    mDefaultKiBAllocationPerPart = defaulBlocksAllocationPerPart;
}


void PmmSdSafeLog::initSafeLogStatusStruct(pmmSdAllocationStatusStructType* statusStruct, uint8_t groupLength, uint16_t KiBPerPart)
{
    statusStruct->currentBlock           = 0;
    statusStruct->freeBlocksAfterCurrent = 0;
    statusStruct->groupLength            = groupLength;
    statusStruct->nextFilePart           = 0;
    
    statusStruct->currentPositionInBlock = 0;

    if (KiBPerPart == 0)
        statusStruct->KiBPerPart      = mDefaultKiBAllocationPerPart;

    else
        statusStruct->KiBPerPart      = KiBPerPart;
}


// READ: The statusStruct must be initSafeLogStatusStruct() before.
// The maximum groupLength is 255, and the code below is written with that in mind, and assuming the block size is 512 bytes.
// On the future it can be improved. Not hard.

// https://stackoverflow.com/questions/31256206/c-memory-alignment
// readBlock() and writeBlock() uses a local array of 512 bytes, and a memcpy() if it is requested.
// When time is available for me, I will write a function in pmmSd for reading and writing without these slowers, and making sure the mBlockBuffer is aligned to 4.
int PmmSdSafeLog::write(uint8_t data[], char dirFullRelativePath[], pmmSdAllocationStatusStructType* statusStruct)
{

    // I didn't want to create this variable and keep changing it, and I didn't want to use (PMM_SD_BLOCK_SIZE - statusStruct->currentPositionInBlock),
    // so I will use a define, which certainly I will regret in the future, but now, is the best option I can think.
    #define remainingBytesInThisBlock_macro (PMM_SD_BLOCK_SIZE - statusStruct->currentPositionInBlock)

    unsigned dataBytesRemaining = statusStruct->groupLength;
    unsigned hadWrittenGroupHeader = false;



    // 1) Is this the first time running for this statusStruct?
    if (statusStruct->currentBlock == 0)
    {
        // The function below will also change some struct member values. Read the corresponding function definition.
        allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, statusStruct);
    }


    // 2) Is the current block full?
    //  The behavior of the (statusStruct->currentPositionInBlock > PMM_SD_BLOCK_SIZE) case is probably horrible. It normally won't happen.
    else if (statusStruct->currentPositionInBlock >= PMM_SD_BLOCK_SIZE)
    {
        if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, statusStruct))
        {
            PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at nextBlockAndAllocIfNeeded() (Partial Final Data), in write()!");
            return 1;
        }
        statusStruct->currentPositionInBlock = 0;
    }

    // 3) If there is something in the actual block, we must first copy it to add the new data.
    else if (statusStruct->currentPositionInBlock > 0)
    {
        if (!mSdioCard->readBlock(statusStruct->currentBlock, mBlockBuffer));
        {
            PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at readBlock(), in write()!");
            return 1;
        }


        // We have two possible cases:
        //    4) If the new data needs another block. -- Partial Initial Data
        //    5) If the new data fits the current block, entirely. -- Entire Data / Partial Final Data

        // 4) Now we will see if the data needs another block. If the conditional is true, we need another block!
        if ((statusStruct->groupLength + 2) >= remainingBytesInThisBlock_macro) // +2 for header and footer
        {

            // 4.1) We first add the group header, which will always fit the current block (as the currentPositionInBlock goes from 0 to 511, if >=512,
            //  it was treated on 2).)
            mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;
            hadWrittenGroupHeader = true;


            // 4.2) Now we add the data to the current block, if there is available space.
            if (remainingBytesInThisBlock_macro) // Avoids useless calls of memcpy
            {
                memcpy(mBlockBuffer + statusStruct->currentPositionInBlock, data, remainingBytesInThisBlock_macro);
                dataBytesRemaining -= remainingBytesInThisBlock_macro;


                // 4.3) Write the Partial Initial Data to the SD.
                if(!mSdioCard->writeBlock(statusStruct->currentBlock, mBlockBuffer))
                {
                    PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at writeBlock() (Partial Initial Data), in write()!");
                    return 1;
                }
            }

            // 4.4) As we filled the current block, we need to move to the next one.
            if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, statusStruct))
            {
                PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at nextBlockAndAllocIfNeeded() (Partial Initial Data), in write()!");
                return 1;
            }
                
        } // End of 4).
    } // End of 3).


    // 5) Write the block that fits entirely the block / Write the Partial Final Data.

    // 5.1) Write the Written Flag if it is a new block.
    if (statusStruct->currentPositionInBlock == 0)
        mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN;


    // 5.2) Write the Group Begin Flag, if not done already at 3.1).
    if (!hadWrittenGroupHeader)
        mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;


    // 5.3) Write the Entire Data, or the Partial Final Data. We check if there is still dataBytesRemaining, as the 3)
    // may only needs to write the Group End Flag.
    if (dataBytesRemaining)
    {
        memcpy(mBlockBuffer + statusStruct->currentPositionInBlock, data + (statusStruct->groupLength - dataBytesRemaining), dataBytesRemaining);

        statusStruct->currentPositionInBlock += dataBytesRemaining;
        dataBytesRemaining = 0;
    }


    // 5.4) Write the Group End Flag.
    mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_END;


    // 5.5) Erase any previous garbage after the written data.
    if (remainingBytesInThisBlock_macro)
        memset(mBlockBuffer + remainingBytesInThisBlock_macro, 0, remainingBytesInThisBlock_macro);


    // 5.6) Write the Last Valid Block to the SD.
    if(!mSdioCard->writeBlock(statusStruct->currentBlock++, mBlockBuffer))
    {
        PMM_DEBUG_ADV_PRINTLN("Error at writeBlock() (@ Last Valid Block)!");
        return 1;
    }
    statusStruct->freeBlocksAfterCurrent--;


    // 6) We need two blocks for the Backup Block 0 and Backup Block 1. As in this system we first write in the Backup Block 1
    //  and only then on the Backup Block 0, and it's possible that the Backup Block 1 is on another file part,
    //  we will get the address of the Backup Block 0, write on the Backup Block 1, and then, on the Backup Block 0.
    
    // 6.1) It only happens when freeBlocksAfterCurrent < 2.
    if (statusStruct->freeBlocksAfterCurrent < 2)
    {
        // 6.1.1) Save current variables of statusStruct. This is necessary as if the part changed on the situation presented above, these
        //  variables will also change.
        uint32_t tempCurrentBlock = statusStruct->currentBlock;
        uint32_t tempFreeBlocks   = statusStruct->freeBlocksAfterCurrent;
        uint32_t tempNextPart     = statusStruct->nextFilePart;


        // 6.1.2) Go to the next block.
        if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, statusStruct))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at nextBlockAndAllocIfNeeded() (@ Backup Block 0)!");
            return 1;
        }


        // 6.1.3) Get the address of this block, the Backup Block 0, as the Backup Block 1 may be on another part.
        uint32_t backupBlock0Address = statusStruct->currentBlock;


        // 6.1.4) Go to the next block.
        if (nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_FILENAME_EXTENSION, statusStruct))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at nextBlockAndAllocIfNeeded() (@ Backup Block 1)!");
            return 1;
        }


        // 6.1.5) Write the Backup Block 1.
        if(!mSdioCard->writeBlock(statusStruct->currentBlock++, mBlockBuffer))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at writeBlock() (@ Backup Block 1)!");
            return 1;
        }
        statusStruct->freeBlocksAfterCurrent--;


        // 6.1.6) Write the Backup Block 0.
        if(!mSdioCard->writeBlock(backupBlock0Address, mBlockBuffer))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at writeBlock() (@ Backup Block 0)!");
            return 1;
        }
        statusStruct->freeBlocksAfterCurrent--;


        // 6.1.7) Return the previous values
        statusStruct->currentBlock           = tempCurrentBlock + 1;
        statusStruct->freeBlocksAfterCurrent = tempFreeBlocks   - 1;
        statusStruct->nextFilePart           = tempNextPart        ;

    } // End of 6.1); if (statusStruct->freeBlocksAfterCurrent < 2)

    // 6.2) If we still have plenty of free blocks in this part
    else 
    {
         // 6.2.1) Write the Backup Block 1.
        if(!mSdioCard->writeBlock(statusStruct->currentBlock++, mBlockBuffer))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at writeBlock() (@ Backup Block 1)!");
            return 1;
        }
        statusStruct->freeBlocksAfterCurrent--;


        // 6.2.2) Write the Backup Block 0.
        if(!mSdioCard->writeBlock(statusStruct->currentBlock++, mBlockBuffer))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at writeBlock() (@ Backup Block 0)!");
            return 1;
        }
        statusStruct->freeBlocksAfterCurrent--;

    } // End of 6.2)
    return 0;
}

const char* PmmSdSafeLog::getFilenameExtension()
{
    return PMM_SD_SAFE_LOG_FILENAME_EXTENSION;
}