#include <stdint.h> // for uint32_t
#include "pmmSd/sd.h"
#include "pmmModules/messageLog/messageLog.h" // antes era: #include <pmmPackages/pmmPortString.h>



int PmmModuleMessageLog::loadSelfString(char stringDestination[], uint16_t requestedStringId)
{
    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];

    mPmmSdPtr->getSelfDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, MESSAGE_LOG_FILENAME);

    if (!mPmmSdPtr->open(tempFilename)) // open returns bool
        return 1;

    for (uint16_t stringId = 0; stringId <= requestedStringId; stringId++)
        if (mPmmSdPtr->fgets(stringDestination, MESSAGE_LOG_MAX_STRING_LENGTH_STORAGE) <= 0)
            return 2;

    return 0;
}

int PmmModuleMessageLog::loadReceivedString(uint8_t sourceAddress, uint8_t sourceSession, char stringDestination[], uint16_t requestedStringId)
{
    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];

    mPmmSdPtr->getReceivedDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, MESSAGE_LOG_FILENAME);

    if (!mPmmSdPtr->open(tempFilename)) // open returns bool
        return 1;

    for (uint16_t stringId = 0; stringId <= requestedStringId; stringId++)
        if (mPmmSdPtr->fgets(stringDestination, MESSAGE_LOG_MAX_STRING_LENGTH_STORAGE) <= 0)
            return 2;

    return 0;
}

int PmmModuleMessageLog::saveSelfString(char string[])
{
    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];

    mPmmSdPtr->getSelfDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, MESSAGE_LOG_FILENAME);
    mPmmSdPtr->createDirsAndOpen(tempFilename);
    mPmmSdPtr->println(string);

    mPmmSdPtr->getSelfDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, MESSAGE_LOG_FILENAME_BACKUP);
    mPmmSdPtr->createDirsAndOpen(tempFilename);
    mPmmSdPtr->println(string);

    return 0;
}

int PmmModuleMessageLog::saveReceivedString(uint8_t sourceAddress, uint8_t sourceSession, char string[])
{
    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];

    mPmmSdPtr->getReceivedDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, MESSAGE_LOG_FILENAME);
    mPmmSdPtr->createDirsAndOpen(tempFilename);
    mPmmSdPtr->println(string);

    mPmmSdPtr->getReceivedDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, MESSAGE_LOG_FILENAME_BACKUP);
    mPmmSdPtr->createDirsAndOpen(tempFilename);
    mPmmSdPtr->println(string);

    return 0;
}