#include <string.h>
#include <pmmConsts.h>

#include "pmmSd/pmmSd.h"
#include "pmmSd/pmmSdSafeLog.h"

// These are files which are:
// 1) Pre-allocated with a length of X blocks - (use the kibibytesToBlocksAmount() or the mebibytesToBlocksAmount() functions to !)
//  The pre-allocation will also erase previous garbages. So, the blocks will be filled with zeroes or ones, deppending on the flash memory vendor.
// 2) Separated in parts. Each part, will have the length of the previous item.
// 3) Each write, have the same length -- maybe for now?
// 4)

// It won't use SdFatSdioEX as it uses multi-block writing, and by doing it, we lose the control of the safety system.

// By having the backup blocks always ahead of the current block instead of a fixed place for them, we distribute the SD


PmmSdSafeLog::PmmSdSafeLog(PmmSd *pmmSd, uint16_t defaulBlocksAllocationPerPart) // To use multiple buffers
{
    mPmmSd = pmmSd;
    mDefaultKiBAllocationPerPart = defaulBlocksAllocationPerPart;
}


void PmmSdSafeLog::initSafeLogStatusStruct(pmmSdAllocationStatusStructType * statusStruct, uint8_t groupLength, uint16_t KiBPerPart)
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


// The statusStruct must be initSafeLogStatusStruct() before.
// The maximum groupLength is 255, and the code below is written with that in mind, and assuming the block size is 512 bytes.
// On the future it can be improved. Not hard.

// //https://stackoverflow.com/questions/31256206/c-memory-alignment
// readBlock() and writeBlock() uses a local array of 512 bytes, and a memcpy() if it is requested.
// When time is available for me, I will write a function in pmmSd for reading and writing without these slowers, and making sure the mBlockBuffer is aligned to 4.
int PmmSdSafeLog::write(uint8_t data[], char dirFullRelativePath[], pmmSdAllocationStatusStructType* statusStruct)
{

    // I didn't want to create this variable and keep changing it, and I didn't want to use (PMM_SD_BLOCK_SIZE - statusStruct->currentPositionInBlock),
    // so I will use a define, which certainly I will regret in the future, but now, is the best option I can think.
    #define remainingBytesInThisBlock_macro (PMM_SD_BLOCK_SIZE - statusStruct->currentPositionInBlock)

    unsigned dataBytesRemaining = statusStruct->groupLength;

    unsigned hadWrittenGroupHeader = false;

    uint32_t backupBlock0Address;


    // 1) Is this the first time running for this statusStruct?
    if (statusStruct->currentBlock == 0)
    {
        // The function below will also change some struct member values. Read the corresponding function definition.
        mPmmSd->allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct);
    }


    // 2) If there is something in the actual block, we must first copy it to add the new data.
    if (statusStruct->currentPositionInBlock > 0)
    {
        if (!mPmmSd->getCard()->readBlock(statusStruct->currentBlock, mBlockBuffer));
        {
            PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at readBlock(), in write()!");
            return 1;
        }

        // We have two possible cases:
        //    3) If the new data needs another block. -- Partial Initial Data
        //    5) If the new data fits the current block, entirely. -- Entire Data / Partial Final Data

        // 3) Now we will see if the data needs another block. If the conditional is true, we need another block!
        if ((statusStruct->groupLength + 2) >= remainingBytesInThisBlock_macro) // +2 for header and footer
        {

            // 3.1) We first add the group header, which will always fit the current block (as the currentPositionInBlock goes from 0 to 511)
            mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;
            hadWrittenGroupHeader = true;

            // 3.2) Now we add the data to the current block, if there is available space.
            if (remainingBytesInThisBlock_macro) // Avoids useless calls of memcpy
            {
                memcpy(mBlockBuffer + statusStruct->currentPositionInBlock, data, remainingBytesInThisBlock_macro);
                dataBytesRemaining -= remainingBytesInThisBlock_macro;

                // 3.3) Write the Partial Initial Data to the SD.
                if(!mPmmSd->getCard()->writeBlock(statusStruct->currentBlock, mBlockBuffer))
                {
                    PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at writeBlock() (Partial Initial Data), in write()!");
                    return 1;
                }
            }

            // 3.4) As we filled the current block, we need to move to the next one.
            if (mPmmSd->nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct))
            {
                PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at nextBlockAndAllocIfNeeded() (Partial Initial Data), in write()!");
                return 1;
            }
                
        } // END of 3).
    } // END of 2).


    // 4) Write the block that fits entirely the block / Write the Partial Final Data.

    // 4.1) Write the Written Flag if it is a new block.
    if (statusStruct->currentPositionInBlock == 0)
        mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN;

    // 4.2) Write the Group Begin Flag, if not done already at 3.1).
    if (!hadWrittenGroupHeader)
        mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;

    // 4.3) Write the Entire Data, or the Partial Final Data. We check if there is still dataBytesRemaining, as the 3)
    // may only needs to write the Group End Flag.
    if (dataBytesRemaining)
    {
        memcpy(mBlockBuffer + statusStruct->currentPositionInBlock, data + (statusStruct->groupLength - dataBytesRemaining), dataBytesRemaining);

        statusStruct->currentPositionInBlock += dataBytesRemaining;
        dataBytesRemaining = 0;
    }

    // 4.4) Write the Group End Flag.
    mBlockBuffer[statusStruct->currentPositionInBlock++] = PMM_SD_ALLOCATION_FLAG_GROUP_END;

    // 4.5) Erase any previous garbage after the written data.
    if (remainingBytesInThisBlock_macro)
        memset(mBlockBuffer + remainingBytesInThisBlock_macro, 0, remainingBytesInThisBlock_macro);

    // 4.6) Write the Last Data Block to the SD.
    if(!mPmmSd->getCard()->writeBlock(statusStruct->currentBlock, mBlockBuffer))
    {
        PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at writeBlock() (Last Valid Block), in write()!");
        return 1;
    }

    // We need two blocks for the Backup Block 0 and Backup Block 1.

    if (mPmmSd->nextBlockAndAllocIfNeeded(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct))
    {
        PMM_DEBUG_PRINTLN("PmmSdSafeLog: Error at nextBlockAndAllocIfNeeded() (Partial Final Data), in write()!");
        return 1;
    }

}
