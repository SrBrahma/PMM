#include "pmmModules/dataLog/dataLog.h"



int PmmModuleDataLog::getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[])
{
    if (!destination)
        return 1;

    if (!additionalPath || additionalPath[0] == '\0')
        // %hu or %u? https://stackoverflow.com/a/8702610/10247962
        snprintf(destination, maxLength, "DataLog ID-%u L-%u", dataLogId, groupLength);

    else
        snprintf(destination, maxLength, "DataLog ID-%u L-%u/%s", dataLogId, groupLength, additionalPath);

    return 0;
}



int PmmModuleDataLog::saveDataLog(uint8_t groupData[], char dirRelativePath[], PmmSdAllocStatus* allocStatus)
{
    return mPmmSdSafeLog->write(groupData, dirRelativePath, allocStatus);
}



int PmmModuleDataLog::saveOwnDataLog()
{
    if (!mPmmSd->getSdIsWorking())
        return 1;

    if (!mIsLocked)
        updateLogInfoCombinedPayload();

    // Build the entire group in a single array
    unsigned currentLength = 0;

    for (unsigned actualVar = 0; actualVar < mNumberVariables; actualVar++)
    {
        memcpy(mGroupTempData + currentLength, mVariableAdrsArray[actualVar], mVariableSizeArray[actualVar]);
        currentLength += mVariableSizeArray[actualVar];
    }

    return saveDataLog(mGroupTempData, mDataLogSelfDirPath, &mAllocStatusSelfDataLog);
}



int PmmModuleDataLog::saveReceivedDataLog(uint8_t groupData[], uint8_t groupLength, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession)
{
    if (!mPmmSd->getSdIsWorking())
        return 1;
    
    if (!groupData)
        return 2;

    // 1) Check if needs to set/reset the statusStruct
    if (mAllocStatusReceived[sourceAddress].currentBlock == 0 || mAllocStatusReceivedSession[sourceAddress] != sourceSession)
    {
        mAllocStatusReceived[sourceAddress].reset(groupLength);
        mAllocStatusReceivedSession[sourceAddress] = sourceSession;
    }

    getDataLogDirectory(mTempFilename, PMM_SD_FILENAME_MAX_LENGTH, dataLogId, groupLength);
    mPmmSd->getReceivedDirectory(mTempFilename2, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, mTempFilename);

    return saveDataLog(groupData, mTempFilename2, &mAllocStatusReceived[sourceAddress]);
}