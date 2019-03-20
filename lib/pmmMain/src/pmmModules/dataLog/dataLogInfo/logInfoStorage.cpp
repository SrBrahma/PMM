#include <byteSelection.h>
#include "pmmDebug.h"
#include "pmmModules/dataLog/dataLog.h"


// This is automatically called by updateLogInfoCombinedPayload().
// You don't need to call this directly.
int PmmModuleDataLog::saveOwnDataLogInfo()
{
    if (!mPmmSd->getSdIsWorking())
        return 1;

    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];
    snprintf(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s", mDataLogSelfDirPath, mLOG_INFO_FILENAME);

    if (!mPmmSd->exists(tempFilename))
    {
        mPmmSd->createDirsAndOpen(tempFilename);
        mPmmSd->write(mLogInfoContentArray, mLogInfoContentArrayLength);
        mPmmSd->close();
    }
    return 0;
}



int PmmModuleDataLog::saveReceivedDataLogInfo(uint8_t data[], uint16_t dataLength, uint8_t sourceAddress, uint8_t sourceSession, uint8_t dataLogId, uint8_t groupLength, uint8_t currentPart, uint8_t totalParts)
{
    if (!mPmmSd->getSdIsWorking())
        return 1;

    if (!data)
        return 2;

    char path[PMM_SD_FILENAME_MAX_LENGTH];
    char pathTemp[PMM_SD_FILENAME_MAX_LENGTH];

    getDataLogDirectory(pathTemp, PMM_SD_FILENAME_MAX_LENGTH, dataLogId, groupLength, mLOG_INFO_FILENAME);
    mPmmSd->getReceivedDirectory(path, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, pathTemp);

    if (!mPmmSd->exists(path)) // If the final file doesn't exists, try to save this part! However, in savePart it will check if we already own this part!
    {
        mPmmSd->getSplit()->savePart(path, data, dataLength, currentPart, totalParts, PMM_SD_SPLIT_BUILD_FLAG); // PMM_SD_SPLIT_BUILD_FLAG will automatically build the final file!
    }

    return 0;
}
