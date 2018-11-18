#include <string.h>


// These are files which are:
// 1) Pre-allocated with a length of X blocks - (use the kibibytesToBlocksAmount() or the mebibytesToBlocksAmount() functions to !)
//  The pre-allocation will also erase previous garbages. So, the blocks will be filled with zeroes or ones, deppending on the flash memory vendor.
// 2) Separated in parts. Each part, will have the length of the previous item.
// 3) Each write, have the same length -- maybe for now?
// 4)

// It won't use SdFatSdioEX as it uses multi-block writing, and by doing it, we lose the control of the safety system.

// By having the backup blocks always ahead of the current block instead of a fixed place for them, we distribute the SD

PmmSdSafeLog(PmmSd *pmmSd, uint16_t defaultBlocksAllocationPerPart)
{
    mPmmSd = pmmSd;
    mDefaultBlocksAllocationPerPart = defaultBlocksAllocationPerPart;
}


int PmmSdSafeLog::write(uint8_t data[], char dirRelativeFullPath[], safeLogStatusStructType* safeLogStatusStruct)
{

}

int PmmSdSafeLog::writeOwn(uint8_t data[], char dirRelativePath[], safeLogStatusStructType* safeLogStatusStruct)
{
    getFilenameOwn
}

int PmmSdSafeLog::writeReceived(uint8_t data[], char dirRelativePath[], uint8_t sourceAddress, uint8_t sourceSession, safeLogStatusStructType* safeLogStatusStruct);
{
    
    if (safeLogStatusStruct->freeBlocks == 0)
    {
    }
    mPmmSd->getFilenameReceived(mDirFullRelativePath, PMM_SD_FILENAME_INTERNAL_MAX_LENGTH, sourceAddress, sourceSession, char filename[]);
}

int PmmSdSafeLog::writeOnBlock(uint8_t data[], uint8_t dataLength, safeLogStatusStructType* safeLogStatusStruct);
{

}