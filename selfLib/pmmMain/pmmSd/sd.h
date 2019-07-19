/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <SdFat.h>
#include "pmmConsts.h"
#include "pmmSd/consts.h"


#define PMM_SD_BASE_DIRECTORY               "PMM"



class PmmSd
{

public:

    PmmSd();

    int init(uint8_t sessionId);

    // Set as current directory the default PMM directory, using the given Session and this system name.
    // PMM/$SYSTEM_NAME$/Session $SESSION_ID$
    int setPmmCurrentDirectory(uint8_t sessionId);

    // For removing directories recursively.
    int removeDirRecursively(char relativePath[]);

    // Returns by the first arg the string of the 'Self' directory, the directory of 'this' system.
    int getSelfDirectory    (char destination[], uint8_t maxLength);

    // Returns by the first arg the string of the 'Received' directory, the directory path using
    // the source address and session.
    int getReceivedDirectory(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession);


    bool     exists(char filename[]);

    // Create directories if they don't exist and open the file with the given parameters.
    int      createDirsAndOpen(File *file, const char path[], oflag_t mode = O_RDWR | O_CREAT);

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

    bool     getSdIsWorking();
    bool     getSdIsBusy();

    SdFatSdio   *getSdFatPtr();
    SdioCard    *getCardPtr ();
    File        *getFile    ();


private:

    bool      mSdIsWorking;

    SdFatSdio mSdFat;
    File      mFile;

    char      mPmmDirPath[PMM_SD_FILENAME_MAX_LENGTH];

    uint8_t   mSessionId;

};

#endif
