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
    uint32_t currentBlock;               // At which block we currently are. If it is 0, it hasn't been allocated yet.
    uint16_t freeBlocksAfterCurrent;     // The absoluteEndBlock is actualBlock + freeBlocks. Even if it is 0, there may still have free bytes on the actual block.
    uint8_t  groupLength;                // Max group length is 255 bytes. On the future it may be extended. But not needed right now.
    uint8_t  currentPart;                // Maximum of 255 parts. Just a quicker way to get the next part filename instead of reading each part's filename.
    uint16_t blocksPerPart;              // Max of 65535 blocks, maximum allocation per part is 32 MiB (64KiB * 512B)
    uint16_t currentPositionInBlock;     // There could be a bit field to 9, but currently, no actual need.
} pmmSdAllocationStatusStructType;               // Total struct size is 12 bytes, with padding.



class PmmSd
{

public:

    PmmSd();
    
    int init(PmmErrorsCentral* pmmErrorsCentral, uint8_t sessionId);

    int println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession = 0);

    int write(char filename[], char arrayToWrite[], size_t length, uint8_t sourceAddress, uint8_t sourceSession = 0);

    bool getSdIsBusy();

    int writeTextFileWithBackup(char filename[], uint8_t sourceAddress, char stringToWrite[]);

    int allocateFilePart(char dirFullRelativePath[], char filenameExtension[], uint8_t filePart, uint16_t blocksToAllocateInThisPart, uint32_t* beginBlock, uint32_t* endBlock);

    void getFilenameOwn(char destination[], uint8_t maxLength, char filename[]);

    void getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[]);

    SdFatSdio* getSdCard();

private:

    SdFatSdio mSdCard;
    
    File mFile;
    File mAllocationFile; // Just for the allocateFilePart() function.

    PmmErrorsCentral* mPmmErrorsCentral;

    uint8_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string

    char mTempFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];
};

#endif
