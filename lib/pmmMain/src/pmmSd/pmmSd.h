/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>
#include "pmmConsts.h"
#include "pmmSd/pmmSdSafeLog.h"


#define PMM_SD_BASE_DIRECTORY               "Minerva Rockets - PMM"


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

    int  init();
    int  init(uint8_t sessionId);

    int  setPmmCurrentDirectory();
    int  setCurrentDirectory   (char fullPath[]);

    int  removeDirRecursively(char relativePath[]);


    void getFilenameOwn     (char destination[], uint8_t maxLength, char filename[]);
    void getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[]);

    int  open (char filename[], char dirFullRelativePath[], uint8_t mode = O_RDWR | O_CREAT | O_APPEND);
    int  seek (uint32_t position);
    int  read (uint8_t buffer[], size_t numberBytes);
    int  write(uint8_t byte);
    int  write(char arrayToWrite[], size_t length);
    int  close();

    int  println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession = 0);

    bool getSdIsBusy();

    SdFatSdio*    getSdFatPtr();
    SdioCard*     getCardPtr ();
    PmmSdSafeLog* getSafeLog ();



private:

    SdFatSdio mSdFat;
    File      mFile;

    unsigned  mSdIsWorking;

    unsigned  mHasCreatedThisSessionDirectory;

    uint8_t   mThisSessionId;

    PmmSdSafeLog mSafeLog;

};

#endif
