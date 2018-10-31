/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"

#include "pmmSd/pmmSd.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"




PmmSd::PmmSd()
{
}

int PmmSd::init(PmmErrorsCentral* pmmErrorsCentral)
{
    mPmmErrorsCentral = pmmErrorsCentral;

    // 1) Initialize the SD
    if (!mSdEx.begin())
    {
        PMM_DEBUG_PRINT("PmmSd #1: SD init failed!");
        mPmmErrorsCentral->reportErrorByCode(ERROR_SD);
        return 1;
    }
    // 1.1) Make sdEx the current volume.
    mSdEx.chvol();

    // 2) Creates the directory tree.
    mSdEx.mkdir(PMM_SD_BASE_DIRECTORY);
    mSdEx.chdir(PMM_SD_BASE_DIRECTORY);

    // 2.1) Get this session Id.
    // Note: If it reaches the maximum Id (999 when I wrote this), the session Id will be the maximum Id.
    // As it will probably never happen, I made it work this way. If it is a problem on future, it's someone's else problem :)
    for (mThisSessionId = 0; mThisSessionId < PMM_SD_MAX_SESSIONS_ID || mSdEx.exists(mThisSessionName); mThisSessionId++)
        snprintf(mThisSessionName, PMM_SD_FILENAME_MAX_LENGTH, "%s_%03u", PMM_THIS_NAME_DEFINE, mThisSessionId); // %03u to make the file id at least 3 digits.

    mSdEx.mkdir(mThisSessionName);
    mSdEx.chdir(mThisSessionName);

    #if PMM_DEBUG_SERIAL
        Serial.print("This Session name is = \""); Serial.print(mThisSessionName); Serial.println("\"");
    #endif

    PMM_DEBUG_PRINT_MORE("PmmSd: Initialized successfully!");
    return 0;
}


int PmmSd::println(char filename[], uint8_t sourceAddress, char string[])
{
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }
    mFile.close();
    return 0;
}

int PmmSd::writeToFilename(char filename[], char arrayToWrite[], int32_t length)
{
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }
    mFile.close();
    return 0;
}



int PmmSd::writeStringToFilename(char *filename, char *arrayToWrite)
{
    int32_t length;
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    length = strlen(arrayToWrite);
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}



//if (!file.open("dir2/DIR3/NAME3.txt", O_WRITE | O_APPEND | O_CREAT)) {
 // error("dir2/DIR3/NAME3.txt");



SdFatSdioEX PmmSd::getSdEx()
{
    return mSdEx;
}



bool PmmSd::getSdIsBusy()
{
    return mSdEx.card()->isBusy();
}



char* PmmSd::getThisSessionNamePtr()
{
    return mThisSessionName;
}