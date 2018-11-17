/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>

#include "pmmErrorsCentral/pmmErrorsCentral.h"
#include "pmmConsts.h"


#define PMM_SD_BASE_DIRECTORY               "/Minerva Rockets/pmm"
#define PMM_SD_BACKUP_SUFFIX                "_backup"

#define PMM_SD_MAX_SESSIONS_ID              999

#define PMM_SD_FILENAME_MAX_LENGTH          64
#define PMM_SD_FILENAME_SUFFIX_LENGTH       6   // The extension! For example, ".txt"


#define PMM_SD_FILENAME_INTERNAL_MAX_LENGTH (PMM_SD_FILENAME_MAX_LENGTH + PMM_SD_FILENAME_SUFFIX_LENGTH + 5)


#define PMM_SD_BLOCK_SIZE                   512
#define PMM_SD_MAXIMUM_BUFFER_LENTH_KIB     16


#define PMM_SD_PLOG_MAGIC_NUMBER_START      'M' // Mine..
#define PMM_SD_PLOG_MAGIC_NUMBER_END        'A' // rvA!!


#define PMM_SD_MAXIMUM_VARIABLE_LENTGTH     8 // Used in pmmSdLowLevel.cpp

uint16_t kibibytesToBlocksAmount(uint16_t kibibytes); // Kibibyte is 1024 bytes! (kilobyte is 1000 bytes!) https://en.wikipedia.org/wiki/Kibibyte
uint16_t mebibytesToBlocksAmount(uint16_t mebibytes); // Mebibyte is 1024 kibibytes! (megabyte is 1000 kilobytes!) https://en.wikipedia.org/wiki/Mebibyte


class PmmSd
{

public:
    PmmSd();
    
    int init(PmmErrorsCentral* pmmErrorsCentral, uint8_t sessionId);

    int println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession = 0);
    int write(char filename[], char arrayToWrite[], size_t length, uint8_t sourceAddress, uint8_t sourceSession = 0);

    bool getSdIsBusy();

    int writeTextFileWithBackup(char filename[], uint8_t sourceAddress, char stringToWrite[]);

    uint32_t allocateFilePart(File* file, char baseFilename[], char filenameExtension[], uint16_t blocksToAllocateInThisPart);

private:
    SdFatSdio mSdEx;
    File mFile;

    PmmErrorsCentral *mPmmErrorsCentral;

    uint16_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string
};

#endif
