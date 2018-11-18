#include <string.h>
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
    mDefaultBlocksAllocationPerPart = defaulBlocksAllocationPerPart;
}


void PmmSdSafeLog::initSafeLogStatusStruct(safeLogStatusStructType * safeLogStatusStruct, uint8_t groupLength, uint16_t blocksPerPart)
{
    safeLogStatusStruct->actualBlock     = 0;
    safeLogStatusStruct->freeBlocks      = 0;
    safeLogStatusStruct->groupLength     = groupLength;
    safeLogStatusStruct->actualPart      = 0;
    
    safeLogStatusStruct->positionInBlock = 0;

    if (blocksPerPart == 0)
        safeLogStatusStruct->blocksPerPart = mDefaultBlocksAllocationPerPart;
    else
        safeLogStatusStruct->blocksPerPart = blocksPerPart;
}


int PmmSdSafeLog::write(uint8_t data[], char dirRelativeFullPath[], safeLogStatusStructType* safeLogStatusStruct)
{

}


// Write the data to the SD. If the actual block runs out of space, will use the next block.
// However, if there isn't another block in the actual part, this function will return how many bytes still need to be written.
// Returns 0 if everything was written.
int PmmSdSafeLog::writeOnSd(uint8_t data[], uint8_t dataLength, safeLogStatusStructType* safeLogStatusStruct);
{
    safeLogStatusStruct->actualBlock;       // At which block we currently are
    safeLogStatusStruct->freeBlocks;       // The absoluteEndBlock is actualBlock + freeBlocks. Even if it is 0, there may still have free bytes on the actual block.
        uint8_t  groupLength;       // Max group length is 255 bytes. On the future it may be extended. But not needed right now.
        uint8_t  actualPart;       // Maximum of 255 parts. Just a quicker way to get the next part filename instead of reading each part's filename.
        uint16_t blocksPerPart;         // max of 65535 blocks, maximum allocation per part is 32 MiB (64KiB * 512B)
        uint16_t positionInBlock : 9;   // As each block has 512 bytes, we need 9 bits.
}