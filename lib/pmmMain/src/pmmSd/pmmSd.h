/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>

#include "pmmConsts.h"

#include "pmmErrorsCentral/pmmErrorsCentral.h"



#define PMM_SD_BASE_DIRECTORY               "/Minerva Rockets/pmm"
#define PMM_SD_BACKUP_SUFFIX                "_backup"

#define PMM_SD_MAX_SESSIONS_ID              255

#define PMM_SD_FILENAME_MAX_LENGTH          64
#define PMM_SD_FILENAME_SUFFIX_LENGTH       6   // The extension! For example, ".txt"


#define PMM_SD_FILENAME_INTERNAL_MAX_LENGTH (PMM_SD_FILENAME_MAX_LENGTH + PMM_SD_FILENAME_SUFFIX_LENGTH + 5)


#define KIBIBYTE_IN_BYTES                   1024
#define MEBIBYTE_IN_BYTES                   1048576

#define PMM_SD_MAX_PART_KIB                 16384 // Be careful if changing this. Read the comments at pmmSdAllocationStatusStructType.

#define PMM_SD_BLOCK_SIZE                   512
#define PMM_SD_MAXIMUM_BUFFER_LENTH_KIB     16


// These letters bellow are flags used in the SD blocks, in safeLog and fastLog writting modes.
// They could be any byte other than 0x00 or 0xFF (default erase possibilities). To honor my team, I gave those letters.
#define PMM_SD_ALLOCATION_FLAG_GROUP_BEGIN      'M' // Mine..
#define PMM_SD_ALLOCATION_FLAG_GROUP_END        'A' // rvA!!
#define PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN    'R' // Rockets!
#define PMM_SD_ALLOCATION_FLAG_BLOCK_WRITTEN_POSITION   0 // Where it will be on the block.



// Debug
#define PMM_SD_DEBUG_MORE           1

#if PMM_SD_DEBUG_MORE
    #define PMM_SD_DEBUG_PRINT_MORE(x) PMM_DEBUG_PRINT_MORE(x)
#else
    #define PMM_SD_DEBUG_PRINT_MORE(x) PMM_CANCEL_MACRO(x)
#endif



uint16_t kibibytesToBlocksAmount(uint16_t kibibytes); // Kibibyte is 1024 bytes! (kilobyte is 1000 bytes!) https://en.wikipedia.org/wiki/Kibibyte
uint16_t mebibytesToBlocksAmount(uint8_t  mebibytes); // Mebibyte is 1024 kibibytes! (megabyte is 1000 kilobytes!) https://en.wikipedia.org/wiki/Mebibyte



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

class PmmSd
{

public:

    PmmSd();
    
    int init(PmmErrorsCentral* pmmErrorsCentral, uint8_t sessionId);

    int println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession = 0);

    int write(char filename[], char arrayToWrite[], size_t length, uint8_t sourceAddress, uint8_t sourceSession = 0);

    bool getSdIsBusy();

    int writeTextFileWithBackup(char filename[], uint8_t sourceAddress, char stringToWrite[]);

    
    int nextBlockAndAllocIfNeeded(char dirFullRelativePath[], char filenameExtension[], pmmSdAllocationStatusStructType* statusStruct);
    int allocateFilePart(char dirFullRelativePath[], char filenameExtension[], pmmSdAllocationStatusStructType* statusStruct);
    int allocateFilePart(char dirFullRelativePath[], char filenameExtension[], uint8_t filePart, uint16_t kibibytesToAllocate, uint32_t* beginBlock, uint32_t* endBlock);

    void getFilenameOwn(char destination[], uint8_t maxLength, char filename[]);

    void getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[]);

    SdFatSdio* getSdFat();

    SdioCard* getCard();

private:

    SdFatSdio mSdFat;
    
    File mFile;
    File mAllocationFile; // Just for the allocateFilePart() function.

    PmmErrorsCentral* mPmmErrorsCentral;

    uint8_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string

    char mTempFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];
};

#endif
