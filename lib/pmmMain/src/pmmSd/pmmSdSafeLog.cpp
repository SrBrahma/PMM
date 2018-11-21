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
int PmmSdSafeLog::write(uint8_t data[], char dirFullRelativePath[], pmmSdAllocationStatusStructType* statusStruct)
{
    unsigned dataBytesRemaining = statusStruct->groupLength; // + 2; // + 2 from header and footer
    int32_t bytesAvailableInCurrentBlock;

    uint32_t endBlock;

    // 1) Is this the first time running for this statusStruct?
    if (statusStruct->currentBlock == 0)
    {
        // The function below will also change some struct member values. Read the corresponding function definition.
        mPmmSd->allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct);
    }



    // 2) If there is something in the actual block, we must first copy it to add the new data.
    if (statusStruct->currentPositionInBlock > 0)
    {
        if (mPmmSd->getSdCard()->card()->readBlock(statusStruct->currentBlock, mBlockBuffer));
                PMM_DEBUG_PRINT("PmmSdSafeLog: Error at readBlock(), in write()!");
//https://stackoverflow.com/questions/31256206/c-memory-alignment

        bytesAvailableInCurrentBlock = PMM_SD_BLOCK_SIZE - statusStruct->currentPositionInBlock;


        // We have two possible cases:
        //    3) If the new data needs another block.
        //    5) If the new data fits the current block, entirely.

        // 3) Now we will see if the data needs another block. If the conditional is true, we need another block!
        if ((statusStruct->groupLength + 2) >= bytesAvailableInCurrentBlock) // +2 for header and footer
        {

            // 3.1) We first add the group header, which will always fit the current block (as the currentPositionInBlock goes from 0 to 511)
            mBlockBuffer[statusStruct->currentPositionInBlock] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;

            statusStruct->currentPositionInBlock++;
            bytesAvailableInCurrentBlock--;

            // 3.2) Now we add the data to the current block, if there is available space.
            if (bytesAvailableInCurrentBlock) // Avoids useless calls of memcpy -- if bytesAvailableInCurrentBlock == 0.
            {
                memcpy(mBlockBuffer + statusStruct->currentPositionInBlock, data, bytesAvailableInCurrentBlock);
                mPmmSd->getSdCard()
            }

            // 3.3) As we filled the current block, we need to move to the next one. However, we need to check if a new file part is needed.
            if (statusStruct->freeBlocksAfterCurrent) 
                statusStruct->freeBlocksAfterCurrent++; // We don't need a new part!

            else 
                mPmmSd->allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct); // We need a new part! 
                // NOTE this function does currentPositionInBlock = 0; freeBlocksAfterCurrent = endBlock - statusStruct->currentBlock; nextFilePart++
        } // END of 3).
    } // END of 2).


    // 2.1) 
    if (statusStruct->currentPositionInBlock == 0)
    {
        mBlockBuffer[PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN_POSITION] = PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN;
        memcpy(mBlockBuffer + 1, 
    }
       
        else
        {
            // 2.1) Read the current block.
            
            
        }

    // 3) Append the data to the buffer

    } // End of bytesRemaining

    //actualPart;
    //blocksPerPart;

    // Allocate new file part
    statusStruct->currentPart++;
    mPmmSd->allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct->currentPart, statusStruct->KiBPerPart, &(statusStruct->currentBlock), &endBlock);

    statusStruct->freeBlocksAfterCurrent = endBlock - statusStruct->currentBlock;

}
