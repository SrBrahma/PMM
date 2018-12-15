/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"  // For this system name

#include "pmmSd/pmmSd.h"



PmmSd::PmmSd()
    : mSafeLog(getSdFatPtr(), getCardPtr())
{
}

int PmmSd::init()
{
    // 1) Initialize the SD
    if (!mSdFat.begin())
    {
        mSdIsWorking = 0;
        PMM_DEBUG_ADV_PRINTLN("Error at mSdFat.begin()!");
        return 1;
    }

    else
    {
        mSdIsWorking = 1;
        PMM_SD_DEBUG_PRINT_MORE("PmmSd: [M] Initialized successfully.");
        return 0;
    }
    mHasCreatedThisSessionDirectory = 0;
}

int PmmSd::init(uint8_t sessionId)
{
    if (init())
    {
        PMM_DEBUG_ADV_PRINTLN("Error at init()!")
        return 1;
    }

    mThisSessionId = sessionId;
    
    if (setPmmCurrentDirectory())
    {
        PMM_DEBUG_ADV_PRINTLN("Error at setPmmCurrentDirectory()!")
        return 2;
    }

    return 0;

}



// Will rename, if exists, a previous Session with the same name, if this is the first time running this function in this Session.
int PmmSd::setPmmCurrentDirectory()
{
    char fullPath[PMM_SD_FILENAME_MAX_LENGTH];
    char fullPathRenameOld[PMM_SD_FILENAME_MAX_LENGTH];
    unsigned oldCounter = 0;

    snprintf(fullPath, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session %02u", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, mThisSessionId);

    // 1) Make sure we are at root dir
    if (!mSdFat.chdir())
    {
        PMM_DEBUG_ADV_PRINTLN("Error at chdir() to root!")
        return 1;
    }

    // 2) If is the first time running this function and there is an old dir with the same name as the new one about to be created,
    //    rename the old one.
    if (mSdFat.exists(fullPath))
    {
        if (!mHasCreatedThisSessionDirectory)
        {
            do
            {
                snprintf(fullPathRenameOld, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session %02u old %02u", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, mThisSessionId, oldCounter);
                oldCounter++;
            } while (mSdFat.exists(fullPathRenameOld));

            if (!mSdFat.rename(fullPath ,fullPathRenameOld))
            {
                PMM_DEBUG_ADV_PRINTLN("Error at rename()!")
                return 2;
            }
            mHasCreatedThisSessionDirectory = 1;
        }
    }
    
    // 3) Create the new dir
    else if (!mSdFat.mkdir(fullPath))
    {
        PMM_DEBUG_ADV_PRINTLN("Error at mkdir()!")
        return 3;
    }

    // 4) Change to it
    if (!mSdFat.chdir(fullPath))
    {
        PMM_DEBUG_ADV_PRINTLN("Error at chdir() to fullPath!")
        return 4;
    }

    return 0;
}

int PmmSd::setCurrentDirectory(char fullPath[])
{
    if (!fullPath)  // Null address
        return 1;

    mSdFat.chdir();
    mSdFat.mkdir(fullPath);
    mSdFat.chdir(fullPath);
    return 0;
}

int PmmSd::removeDirRecursively(char relativePath[])
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

// sourceAddress is from where we did receive the message, for example, the PMM_TELEMETRY_ADDRESS_SELF define.
// sourceSession is from which session we did receive the message. This field is useless if the soureAddress is the
//  PMM_TELEMETRY_ADDRESS_SELF define, so it's an optional argument.
int PmmSd::println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession)
{
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    if (strlen(string) + 2 != mFile.println(string)) // + 2 as it will write the normal chars from the string, plus the '\r' and the '\n' (they are added in the file.println().
    {                                                // comparing to -1 the result is problematic, as the println function returns a sum of the write(string) + write("\r\n"),
                                                     // so it's possible to return a false negative.
        PMM_DEBUG_PRINTLN("PmmSd: ERROR 2 - The string haven't been successfully written!");
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}



int PmmSd::open(char filename[], char dirFullRelativePath[], uint8_t mode)
{
    // Create directories if doesn't exists
    if (!mSdFat.exists(dirFullRelativePath))
        mSdFat.mkdir(dirFullRelativePath);

    if (!mFile.open(filename, mode)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }
    return 0;
}
int PmmSd::seek(uint32_t position)
{
    return mFile.seek(position);
}
int PmmSd::read(uint8_t buffer[], size_t numberBytes)
{
    if (!buffer)
        return 1;

    return mFile.read(buffer, numberBytes);
}
int PmmSd::write(uint8_t byte)
{
    return mFile.write(byte);
}
int PmmSd::write(char arrayToWrite[], size_t length)
{
    return mFile.write(arrayToWrite, length);
}
int PmmSd::close()
{
    return mFile.close();
}



void PmmSd::getFilenameOwn(char destination[], uint8_t maxLength, char filename[])
{
    snprintf(destination, maxLength, "_self/%s", filename);
}



void PmmSd::getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[])
{
    snprintf(destination, maxLength, "%03u/%02u/%s", sourceAddress, sourceSession, filename);
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

bool PmmSd::getSdIsBusy()
{
    return mSdFat.card()->isBusy();
}