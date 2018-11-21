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
// This can be changed, but will leave it to the future. So much stuff to do, alone.

int PmmSdSafeLog::write(uint8_t data[], char dirFullRelativePath[], pmmSdAllocationStatusStructType* statusStruct)
{
    unsigned dataBytesRemaining = statusStruct->groupLength; // + 2; // + 2 from header and footer
    uint32_t endBlock;

    // 1) Is this the first time running for this statusStruct?
    if (statusStruct->currentBlock == 0)
    {
        // The function below will also change some struct member values. Read the corresponding function definition.
        mPmmSd->allocateFilePart(dirFullRelativePath, PMM_SD_SAFE_LOG_EXTENSION, statusStruct);
    }

    // We have two possible cases:
    //  If the new data fits the current block, entirely.
    //  If the new data needs another block.

    // 2) Anyway, if there is something in the actual block, we must first copy it to add the new data.
    if (statusStruct->currentPositionInBlock > 0)
    {
        if (mPmmSd->getSdCard()->card()->readBlock(statusStruct->currentBlock, mBlockBuffer));
                PMM_DEBUG_PRINT("PmmSdSafeLog: Error at readBlock(), in write()!");
        
        // 3) Now we will see if the data needs another block. If the conditional is true, we need another block!
        if ((statusStruct->currentPositionInBlock + statusStruct->groupLength + 2) >= PMM_SD_BLOCK_SIZE)
        {
            // 3.1) We first add the group header, which will always fit the current block (as the currentPositionInBlock goes from 0 to 511)
            mBlockBuffer[statusStruct->currentPositionInBlock] = PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN;
            
            // 3.2) Increase the currentPositionInBlock. (But first, we check if the next position will be on the next block!)
            if (statusStruct->currentPositionInBlock++ >= PMM_SD_BLOCK_SIZE)
            {
                if (statusStruct->freeBlocksAfterCurrent 
                memcpy()

    // 2.1) Was there any previous data written on the actual block?
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
