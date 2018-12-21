#include <byteSelection.h>
#include "pmmModules/dataLog/dataLog.h"


int PmmModuleDataLog::saveOwnDataLogInfo()
{
    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];
    snprintf(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s/%s", mDataLogSelfDirPath, LOG_INFO_FILENAME);

    if (!mPmmSd->exists(tempFilename))
    {
        mPmmSd->open(tempFilename);
        mPmmSd->write(mLogInfoContentArray, mLogInfoContentArrayLength);
        mPmmSd->close();
    }
    return 0;
}



int PmmModuleDataLog::saveReceivedDataLogInfo(uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession)
{
    int  finishedBuilding;
    char filename[PMM_SD_FILENAME_MAX_LENGTH];
    char filenameTemp[PMM_SD_FILENAME_MAX_LENGTH];

    getDataLogDirectory(filenameTemp, PMM_SD_FILENAME_MAX_LENGTH, mDataLogId, mGroupLength, LOG_INFO_FILENAME);
    mPmmSd->getReceivedDirectory(filename, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, filenameTemp);

    mPmmSd->getSplit()->savePart(filename, data, dataLength, currentPart, totalParts);

    return 0;
}
