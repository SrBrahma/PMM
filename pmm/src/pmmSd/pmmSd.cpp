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

int PmmSd::init(PmmErrorsCentral* pmmErrorsCentral, uint8_t sessionId)
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



    PMM_DEBUG_PRINT_MORE("PmmSd: Initialized successfully!");
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
        PMM_DEBUG_PRINT("PmmSd #2: The string haven't been successfully written!");
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
        PMM_DEBUG_PRINT("PmmSd #3: The data haven't been successfully written!");
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}



SdFatSdioEX PmmSd::getSdEx()
{
    return mSdEx;
}



bool PmmSd::getSdIsBusy()
{
    return mSdEx.card()->isBusy();
}