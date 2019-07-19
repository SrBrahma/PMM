/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"                      // For this system name
#include "pmmDebug.h"
#include "pmmSd/generalFunctions.h"
#include "pmmSd/sd.h"



PmmSd::PmmSd() {}

int PmmSd::init(uint8_t sessionId) {
    mSdIsWorking = 0;

    // 1) Initialize the SD
    if (!mSdFat.begin()) {        
        advPrintf("Error at mSdFat.begin()!\n");
        return 1;
    }

    mPmmDirPath[0] = '\0';

    mSessionId = sessionId;
    if (setPmmCurrentDirectory(mSessionId))  return 2;

    mSdIsWorking   = 1;
    sdDebugMorePrintf("PmmSd: [M] Initialized successfully.\n");
    return 0;
}




int  PmmSd::setPmmCurrentDirectory(uint8_t sessionId) {
    // 1) Make sure we are at root dir
    if (!mSdFat.chdir()) {
        advPrintf("Error at chdir() to root!\n")
        return 1;
    }

    // 2) Did we already created the Session dir at this system session?
    if (mPmmDirPath[0] == '\0') {
        // 2.1) No! So, create it!
        snprintf(mPmmDirPath, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session %02hu", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, sessionId);

        // 2.2) If the directory name already exists, create a new one with a suffix.
        int counter = 1;
        while (mSdFat.exists(mPmmDirPath)) 
            snprintf(mPmmDirPath, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session %02u_%u", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, sessionId, counter++);
    }

    // 3) Create the new dir
    if (!mSdFat.mkdir(mPmmDirPath)) {
        advPrintf("Error at mkdir()!\n")
        return 3;
    }

    // 4) Change to the dir
    if (!mSdFat.chdir(mPmmDirPath)) {
        advPrintf("Error at chdir() to mPmmDirPath!\n")
        return 4;
    }

    return 0;
}



int  PmmSd::removeDirRecursively(char relativePath[]) {
    // open the dir
    if (mSdFat.exists(relativePath)) {
        mFile.open(relativePath, O_RDWR);
        mFile.rmRfStar();
        mFile.close();
    }

    return 0;
}


// The functions below are mostly just a call for the original function. But you won't need to use the File variable directly.
// For using it directly (aka using a function not listed here), you can just getFile() from this class.

bool     PmmSd::exists(char path[]) { return mSdFat.exists(path); }
int      PmmSd::createDirsAndOpen(const char path[], oflag_t mode) {
    return ::createDirsAndOpen(&mSdFat, &mFile, path, mode); // https://stackoverflow.com/a/1061630/10247962
}
int      PmmSd::createDirsAndOpen(File *file, const char path[], oflag_t mode) {
    return ::createDirsAndOpen(&mSdFat, file, path, mode); // https://stackoverflow.com/a/1061630/10247962
}
int      PmmSd::read(uint8_t buffer[], size_t numberBytes) {
    if (!buffer) return 1;
    return mFile.read(buffer, numberBytes);
}
int      PmmSd::open(const char path[], oflag_t mode)                     { return mFile.open(path, mode); }
int      PmmSd::seek(uint32_t position)                                   { return mFile.seek(position); }
int      PmmSd::fgets(char buffer[], int16_t maxLength, char delimiter[]) { return mFile.fgets(buffer, maxLength, delimiter); }
int      PmmSd::write(uint8_t byte)                                       { return mFile.write(byte); }
int      PmmSd::write(uint8_t arrayToWrite[], size_t length)              { return mFile.write(arrayToWrite, length); }
int      PmmSd::println(char buffer[])                                    { return mFile.println(buffer); }
int      PmmSd::close()                                                   { return mFile.close(); }
uint32_t PmmSd::size()                                                    { return mFile.size(); }



int  PmmSd::getSelfDirectory(char destination[], uint8_t maxLength) {
    if (!destination)   return -1;
    return snprintf(destination, maxLength, "%s", PMM_SD_DIRECTORY_SELF);
}

int  PmmSd::getReceivedDirectory(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession) {
    if (!destination)   return -1;
    return snprintf(destination, maxLength, "%03u/Session %02u", sourceAddress, sourceSession);
}



SdioCard*     PmmSd::getCardPtr()  { return mSdFat.card(); }
SdFatSdio*    PmmSd::getSdFatPtr() { return &mSdFat; }
File*         PmmSd::getFile()     { return &mFile; }

bool PmmSd::getSdIsWorking() { return mSdIsWorking;}
bool PmmSd::getSdIsBusy()    { return mSdFat.card()->isBusy(); }