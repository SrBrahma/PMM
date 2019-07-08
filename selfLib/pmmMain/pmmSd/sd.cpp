/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"                      // For this system name
#include "pmmDebug.h"
#include "pmmSd/generalFunctions.h"
#include "pmmSd/sd.h"



PmmSd::PmmSd()
    : mSafeLog(&mSdFat),
      mSplit(&mSdFat)
{
}

int  PmmSd::init()
{
    // 1) Initialize the SD
    if (!mSdFat.begin())
    {
        mSdIsWorking = 0;
        advPrintf("Error at mSdFat.begin()!\n");
        return 1;
    }

    else
    {
        mSdIsWorking = 1;

        mPmmDirPath[0] = '\0';

        sdDebugMorePrintf("PmmSd: [M] Initialized successfully.\n");
        return 0;
    }
}

int  PmmSd::init(uint8_t sessionId)
{
    int returnValue;

    if ((returnValue = init()))
    {
        return returnValue;
    }

    mThisSessionId = sessionId;

    if ((returnValue = setPmmCurrentDirectory()))
    {
        advPrintf("Error at setPmmCurrentDirectory()!\n")
        return returnValue;
    }

    return 0;

}



// Will rename, if exists, a previous Session with the same name, if this is the first time running this function in this Session.
int  PmmSd::setPmmCurrentDirectory()
{

    // 1) Make sure we are at root dir
    if (!mSdFat.chdir())
    {
        advPrintf("Error at chdir() to root!\n")
        return 1;
    }

    // 2) Did we already created the Session dir at this system session?
    if (!mPmmDirPath[0])
    {
        // 3) No! So, create it!
        snprintf(mPmmDirPath, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session %02u", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, mThisSessionId);

        // 4) If the directory name already exists, create a new one with a suffix.
        if (mSdFat.exists(mPmmDirPath))
        {
            unsigned counter = 1;
            do
            {
                snprintf(mPmmDirPath, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session %02u - %02u", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, mThisSessionId, counter);
                counter++;
            } while (mSdFat.exists(mPmmDirPath));
        }
    }

    // 3) Create the new dir
    if (!mSdFat.mkdir(mPmmDirPath))
    {
        advPrintf("Error at mkdir()!\n")
        return 3;
    }

    // 4) Change to the dir
    if (!mSdFat.chdir(mPmmDirPath))
    {
        advPrintf("Error at chdir() to mPmmDirPath!\n")
        return 4;
    }

    return 0;
}

int  PmmSd::setCurrentDirectory(char fullPath[])
{
    if (!fullPath)  // Null address
        return 1;

    mSdFat.chdir();
    mSdFat.mkdir(fullPath);
    mSdFat.chdir(fullPath);
    return 0;
}

int  PmmSd::removeDirRecursively(char relativePath[])
{
    // open the dir
    if (mSdFat.exists(relativePath))
    {
        mFile.open(relativePath, O_RDWR);
            //return 1;

        mFile.rmRfStar();
           // return 2;

        mFile.close();
           // return 3;
    }

    return 0;
}


// The functions below are mostly just a call for the original function. But you won't need to use the File variable directly.
// For using it directly (aka using a function not listed here), you can just getFile() from this class.

bool PmmSd::exists(char path[])
{
    return mSdFat.exists(path);
}

// The default open, but this will also automatically create the path.
int  PmmSd::createDirsAndOpen(const char path[], oflag_t mode)
{
    return ::createDirsAndOpen(&mSdFat, &mFile, path, mode); // https://stackoverflow.com/a/1061630/10247962
}

int  PmmSd::open(const char path[], oflag_t mode)
{
    return mFile.open(path, mode);
}
int  PmmSd::seek(uint32_t position)
{
    return mFile.seek(position);
}
uint32_t PmmSd::size()
{
    return mFile.size();
}
int  PmmSd::read(uint8_t buffer[], size_t numberBytes)
{
    if (!buffer)
        return 1;

    return mFile.read(buffer, numberBytes);
}
int  PmmSd::fgets(char buffer[], int16_t maxLength, char delimiter[])
{
    return mFile.fgets(buffer, maxLength, delimiter);
}
int  PmmSd::write(uint8_t byte)
{
    return mFile.write(byte);
}
int  PmmSd::write(uint8_t arrayToWrite[], size_t length)
{
    return mFile.write(arrayToWrite, length);
}
int  PmmSd::println(char buffer[])
{
    return mFile.println(buffer);
}
int  PmmSd::close()
{
    return mFile.close();
}



int  PmmSd::getSelfDirectory(char destination[], uint8_t maxLength, const char additionalPath[])
{
    if (!destination)
        return 0;

    // Left expression is always evaluated first! https://stackoverflow.com/a/2456415/10247962
    if (!additionalPath || additionalPath[0] == '\0')
        return snprintf(destination, maxLength, "%s", PMM_SD_DIRECTORY_SELF);

    else
        return snprintf(destination, maxLength, "%s/%s", PMM_SD_DIRECTORY_SELF, additionalPath);

    return 0;
}



int  PmmSd::getReceivedDirectory(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, const char additionalPath[])
{
    if (!destination)
        return 1;

    // Left expression is always evaluated first! https://stackoverflow.com/a/2456415/10247962
    if (!additionalPath || additionalPath[0] == '\0')
        snprintf(destination, maxLength, "%03u/Session %02u", sourceAddress, sourceSession);

    else
        snprintf(destination, maxLength, "%03u/Session %02u/%s", sourceAddress, sourceSession, additionalPath);

    return 0;
}



SdioCard*     PmmSd::getCardPtr()
{
    return mSdFat.card();
}
SdFatSdio*    PmmSd::getSdFatPtr()
{
    return &mSdFat;
}
PmmSdSafeLog* PmmSd::getSafeLog()
{
    return &mSafeLog;
}
PmmSdSplit*   PmmSd::getSplit()
{
    return &mSplit;
}
File*         PmmSd::getFile()
{
    return &mFile;
}

unsigned PmmSd::getSdIsWorking()
{
    return mSdIsWorking;
}

bool PmmSd::getSdIsBusy()
{
    return mSdFat.card()->isBusy();
}