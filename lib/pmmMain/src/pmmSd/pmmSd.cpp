/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"  // For this system name

#include "pmmSd/pmmSd.h"



PmmSd::PmmSd()
{
    // 1) Initialize the SD
    if (!mSdFat.begin())
    {
        PMM_DEBUG_ADV_PRINTLN("Sd init failed!");
    }

    PMM_SD_DEBUG_PRINT_MORE("PmmSd: [M] Initialized successfully.");
}

PmmSd::PmmSd(uint8_t sessionId)
{
    PmmSd();
    setPmmCurrentDirectory(sessionId);
}

PmmSd::PmmSd(char fullPath[])
{
    PmmSd();
    setCurrentDirectory(fullPath);
}



int PmmSd::setPmmCurrentDirectory(uint8_t sessionId)
{
    char fullPath[PMM_SD_FILENAME_MAX_LENGTH];
    snprintf(fullPath, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s/Session_%02u", PMM_SD_BASE_DIRECTORY, PMM_THIS_NAME_DEFINE, sessionId);

    mSdFat.chdir(1);
    mSdFat.mkdir(fullPath);
    mSdFat.chdir(fullPath, 1);
    return 0;
}

int PmmSd::setCurrentDirectory(char fullPath[])
{
    if (!fullPath)  // Null address
        return 1;

    mSdFat.chdir(1);
    mSdFat.mkdir(fullPath);
    mSdFat.chdir(fullPath, 1);
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



// sourceAddress is from where we did receive the message, for example, the PMM_TELEMETRY_ADDRESS_SELF define.
// sourceSession is from which session we did receive the message. This field is useless if the soureAddress is the
//  PMM_TELEMETRY_ADDRESS_SELF define, so it's an optional argument.
// length argument is int as the return value of the file.write() is also int, so when comparing the results, the same sign is used.
int PmmSd::write(char filename[], char arrayToWrite[], size_t length, uint8_t sourceAddress, uint8_t sourceSession)
{

    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    if (mFile.write(arrayToWrite, length) == -1)
    {
        PMM_DEBUG_PRINTLN("PmmSd: ERROR 3 - The data haven't been successfully written!");
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}



void PmmSd::getFilenameOwn(char destination[], uint8_t maxLength, char filename[])
{
    snprintf(destination, maxLength, "_self/%s", filename);
}



void PmmSd::getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[])
{
    snprintf(destination, maxLength, "%03u/%02u/%s", sourceAddress, sourceSession, filename);
}



SdioCard* PmmSd::getCardPtr()
{
    return mSdFat.card();
}

SdFatSdio* PmmSd::getSdFatPtr()
{
    return &mSdFat;
}

bool PmmSd::getSdIsBusy()
{
    return mSdFat.card()->isBusy();
}