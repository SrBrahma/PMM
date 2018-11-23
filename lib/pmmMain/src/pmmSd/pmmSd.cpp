/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"

#include "pmmSd/pmmSd.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"

#include "pmmTelemetry/pmmTelemetryProtocols.h" // For PMM_TELEMETRY_ADDRESS_SELF define



uint16_t kibibytesToBlocksAmount(uint16_t kibibytes) // Kibibyte is 1024 bytes! (kilobyte is 1000 bytes!) https://en.wikipedia.org/wiki/Kibibyte
{
    uint32_t tempValue = kibibytes * (1024 / PMM_SD_BLOCK_SIZE);
    if (tempValue > 0xFFFF)
        tempValue = 0xFFFF; // Avoid overflow of the uint16_t
    return tempValue; 
}

uint16_t mebibytesToBlocksAmount(uint8_t mebibytes) // Mebibyte is 1024 kibibytes! (megabyte is 1000 kilobytes!) https://en.wikipedia.org/wiki/Mebibyte
{
    uint32_t tempValue = mebibytes * (1048576 / PMM_SD_BLOCK_SIZE); // 1048576 bytes is 1 MiB
    if (tempValue > 0xFFFF)
        tempValue = 0xFFFF; // Avoid overflow of the uint16_t
    return tempValue; 
}



PmmSd::PmmSd()
{
}

int PmmSd::init(PmmErrorsCentral* pmmErrorsCentral, uint8_t sessionId)
{
    mPmmErrorsCentral = pmmErrorsCentral;

    // 1) Initialize the SD
    if (!mSdFat.begin())
    {
        PMM_DEBUG_PRINTLN("PmmSd: ERROR 1 - SD init failed!");
        mPmmErrorsCentral->reportErrorByCode(ERROR_SD);
        return 1;
    }
    // 1.1) Make sdEx the current volume.
    mSdFat.chvol();

    // 2) Creates the directory tree.
    mSdFat.mkdir(PMM_SD_BASE_DIRECTORY);
    mSdFat.chdir(PMM_SD_BASE_DIRECTORY);

    PMM_SD_DEBUG_PRINT_MORE("PmmSd: [M] Initialized successfully.");
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
    snprintf(destination, maxLength, "%03u/%03u/%s", sourceAddress, sourceSession, filename);
}










SdioCard* PmmSd::getCard()
{
    return mSdFat.card();
}

SdFatSdio* PmmSd::getSdFat()
{
    return &mSdFat;
}

bool PmmSd::getSdIsBusy()
{
    return mSdFat.card()->isBusy();
}