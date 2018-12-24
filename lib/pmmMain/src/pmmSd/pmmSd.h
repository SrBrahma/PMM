/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>
#include "pmmConsts.h"
#include "pmmSd/alloc/pmmSdSafeLog.h"
#include "pmmSd/split/pmmSdSplit.h"

#define PMM_SD_BASE_DIRECTORY               "Minerva Rockets - PMM"


// Debug
#define PMM_SD_DEBUG_MORE           1

#if PMM_SD_DEBUG_MORE
    #define PMM_SD_DEBUG_PRINT_MORE(x) PMM_DEBUG_MORE_PRINTLN(x)
#else
    #define PMM_SD_DEBUG_PRINT_MORE(x) PMM_CANCEL_MACRO(x)(x)
#endif



class PmmSd
{

public:

    PmmSd();

    int init();
    int init(uint8_t sessionId);

    int setPmmCurrentDirectory();
    int setCurrentDirectory   (char fullPath[]);

    int removeDirRecursively(char relativePath[]);


    int getSelfDirectory    (char destination[], uint8_t maxLength, char additionalPath[] = NULL);
    int getReceivedDirectory(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char additionalPath[] = NULL);

    bool     exists(char filename[]);

    int      createDirsAndOpen(char path[], uint8_t mode = O_RDWR | O_CREAT | O_APPEND);
    int      open (char filename[], uint8_t mode = O_READ);
    int      seek (uint32_t position);
    int      read (uint8_t buffer[], size_t numberBytes);
    int      write(uint8_t byte);
    int      write(uint8_t arrayToWrite[], size_t length);
    int      close();
    uint32_t size();

    bool     getSdIsBusy();

    SdFatSdio*    getSdFatPtr();
    SdioCard*     getCardPtr ();
    PmmSdSafeLog* getSafeLog ();
    PmmSdSplit*   getSplit   ();
    File*         getFile    ();


private:

    SdFatSdio mSdFat;
    File      mFile;

    unsigned  mSdIsWorking;

    char      mPmmDirPath[PMM_SD_FILENAME_MAX_LENGTH];

    uint8_t   mThisSessionId;

    PmmSdSafeLog mSafeLog;
    PmmSdSplit   mSplit;
    
    char mTempFilename[PMM_SD_FILENAME_MAX_LENGTH];

};

#endif
