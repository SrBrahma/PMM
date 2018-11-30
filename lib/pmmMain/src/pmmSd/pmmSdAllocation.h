#ifndef PMM_SD_ALLOCATION_h
#define PMM_SD_ALLOCATION_h

#include <stdint.h>     // For uintx_t types
#include <SdFat.h>
#include "pmmSd/pmmSd.h"

#define PMM_SD_ALLOCATION_PART_KIB              16384 // Be careful if changing this. Read the comments at pmmSdAllocationStatusStructType.

// These letters bellow are flags used in the SD blocks, in SafeLog and fastLog writting modes.
// They could be any byte other than 0x00 or 0xFF (default erase possibilities). To honor my team, I gave those letters.
#define PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN      'M' // Mine..
#define PMM_SD_ALLOCATION_FLAG_GROUP_END        'A' // rvA!!

#define PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN    'R' // Rockets!

#define PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN_POSITION   0 // Where it will be on the block.

// These ifndef allows using this file without PMM.

#ifndef PMM_DEBUG_PRINTLN
    #define PMM_DEBUG_PRINTLN(x) Serial.println(x)
#endif


typedef struct
{
    // At which block we currently are. If it is 0, it hasn't been allocated yet.
    uint32_t currentBlock;

    // The absoluteEndBlock is actualBlock + freeBlocks. Even if it is 0, there may still have free bytes on the actual block.
    uint16_t freeBlocksAfterCurrent;

    // Max group length is 255 bytes. On the future it may be extended. But not needed right now.
    uint8_t  groupLength;

    // Maximum of 255 parts. Just a quicker way to get the next part filename instead of reading each part's filename.
    uint8_t  nextFilePart;

    // The desired size for each part. Note that the actual allocated file may have a different value, due to the current card's cluster size.
    // Also, even being a uint16_bit (max 64MiB), I capped (on allocateFilePart()) the maximum size to be 16MiB, to avoid *big* mistakes.
    // You can change the maximum size to 32MiB -- BUT KEEP A LITTLE LESS! As the allocation works by clusters, it may happen that the 
    // freeBlocksAfterCurrent may not be able to store all the blocks, and the system would be messed.
    uint16_t KiBPerPart;

    // There could be a bit field to 9, but currently, no actual need.
    uint16_t currentPositionInBlock;
    
} pmmSdAllocationStatusStructType;      // Total struct size is 12 bytes, with padding.



class PmmSdAllocation
{

public:

    PmmSdAllocation(SdFatSdio* sdFat);

    // Writing
    int  allocateFilePart(char dirFullRelativePath[], const char filenameExtension[], pmmSdAllocationStatusStructType* statusStruct);
    int  allocateFilePart(char dirFullRelativePath[], const char filenameExtension[], uint8_t filePart, uint16_t kibibytesToAllocate, uint32_t* beginBlock, uint32_t* endBlock);

    int  nextBlockAndAllocIfNeeded(char dirFullRelativePath[], const char filenameExtension[], pmmSdAllocationStatusStructType* statusStruct);

    // Reading
    int  getNumberFileParts(char dirFullRelativePath[], const char filenameExtension[], uint8_t* fileParts);

    void getFilePartName(char destinationArray[], char dirFullRelativePath[], uint8_t filePart, const char filenameExtension[]);
    int  getFileRange   (char fileFullRelativePath[], uint32_t *beginBlock, uint32_t *endBlock);

    int  readBlock(uint32_t blockAddress, uint8_t arrayToCopyTheContent[]);  // To be easily used by external classes, for debug etc

protected:

    SdFatSdio* mSdFat;
    SdioCard*  mSdioCard;
    File       mFile;
    char       mTempFilename[PMM_SD_FILENAME_MAX_LENGTH];

};

#endif