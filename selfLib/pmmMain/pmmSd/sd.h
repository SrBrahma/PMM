/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>
#include "pmmConsts.h"
#include "pmmSd/consts.h"


#define PMM_SD_BASE_DIRECTORY               "Minerva Rockets - PMM"



class PmmSd
{

public:

    PmmSd();

    int init(uint8_t sessionId = 0);

    int setPmmCurrentDirectory();
    int setCurrentDirectory   (char fullPath[]);

    int removeDirRecursively(char relativePath[]);

    int getSelfDirectory    (char destination[], uint8_t maxLength, const char additionalPath[] = NULL);
    int getReceivedDirectory(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, const char additionalPath[] = NULL);


    bool     exists(char filename[]);

    int      createDirsAndOpen(const char path[], oflag_t mode = O_RDWR | O_CREAT);
    int      open (const char filename[], oflag_t mode = O_READ);
    int      seek (uint32_t position);
    int      read (uint8_t buffer[], size_t numberBytes);
    int      fgets(char buffer[], int16_t maxLength, char delimiter[] = 0);
    int      write(uint8_t byte);
    int      write(uint8_t arrayToWrite[], size_t length);
    int      println(char buffer[]);
    int      close();
    uint32_t size ();

    unsigned      getSdIsWorking();
    bool          getSdIsBusy();

    SdFatSdio   *getSdFatPtr();
    SdioCard    *getCardPtr ();
    File        *getFile    ();


private:

    bool      mSdIsWorking;

    SdFatSdio mSdFat;
    File      mFile;

    char      mPmmDirPath[PMM_SD_FILENAME_MAX_LENGTH];

    uint8_t   mThisSessionId;

};

#endif
