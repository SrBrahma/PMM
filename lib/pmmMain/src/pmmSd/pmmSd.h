/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>
#include "pmmConsts.h"



#define PMM_SD_BASE_DIRECTORY               "/Minerva Rockets/pmm"


#define PMM_SD_FILENAME_MAX_LENGTH          128


#define PMM_SD_BLOCK_SIZE                   512


#define KIBIBYTE_IN_BYTES                   1024
#define MEBIBYTE_IN_BYTES                   1048576



// Debug
#define PMM_SD_DEBUG_MORE           1

#if PMM_SD_DEBUG_MORE
    #define PMM_SD_DEBUG_PRINT_MORE(x) PMM_DEBUG_PRINTLN_MORE(x)
#else
    #define PMM_SD_DEBUG_PRINT_MORE(x) PMM_CANCEL_MACRO(x)
#endif



class PmmSd
{

public:

    PmmSd();

    int init();
    int init(uint8_t sessionId);

    int setPmmCurrentDirectory();
    int setCurrentDirectory   (char fullPath[]);

    int println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession = 0);

    int write(char filename[], char arrayToWrite[], size_t length, uint8_t sourceAddress, uint8_t sourceSession = 0);


    int writeTextFileWithBackup(char filename[], uint8_t sourceAddress, char stringToWrite[]);

    void getFilenameOwn(char destination[], uint8_t maxLength, char filename[]);

    void getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[]);

    bool getSdIsBusy();

    int removeDirRecursively(char relativePath[]);
    
    SdFatSdio* getSdFatPtr();

    SdioCard* getCardPtr();

private:

    SdFatSdio mSdFat;

    File mFile;

    unsigned mSdIsWorking;

    unsigned mHasCreatedThisSessionDirectory;

    uint8_t mThisSessionId;

};

#endif
