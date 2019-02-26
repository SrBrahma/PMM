#include "pmmModules/dataLog/dataLog.h"



int PmmModuleDataLog::getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[])
{
    if (!destination)
        return 0;

    if (!additionalPath || additionalPath[0] == '\0')
        // %hu or %u? https://stackoverflow.com/a/8702610/10247962
        return snprintf(destination, maxLength, "DataLog ID-%u L-%u", dataLogId, groupLength);

    else
        return snprintf(destination, maxLength, "DataLog ID-%u L-%u/%s", dataLogId, groupLength, additionalPath);
}



int PmmModuleDataLog::saveDataLog(uint8_t groupData[], char dirRelativePath[], PmmSdAllocStatus* allocStatus)
{
    uint32_t millisTime = millis();
    int returnVal = mPmmSdSafeLog->write(groupData, dirRelativePath, allocStatus);
    advPrintf("Time taken to save was %f seconds.\n", (millis() - millisTime) / 1000.0);
    return returnVal;
}



int PmmModuleDataLog::saveOwnDataLog()
{
    if (!mPmmSd->getSdIsWorking())
        return 1;

    if (!lockGroup())
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

    char tempFilename [PMM_SD_FILENAME_MAX_LENGTH];
    char tempFilename2[PMM_SD_FILENAME_MAX_LENGTH];

    // 1) Check if needs to set/reset the statusStruct
    if (mAllocStatusReceived[sourceAddress].currentBlock == 0 || mAllocStatusReceivedSession[sourceAddress] != sourceSession)
    {
        mAllocStatusReceived[sourceAddress].reset(groupLength);
        mAllocStatusReceivedSession[sourceAddress] = sourceSession;
    }

    getDataLogDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, dataLogId, groupLength);
    mPmmSd->getReceivedDirectory(tempFilename2, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, tempFilename);

    return saveDataLog(groupData, tempFilename2, &mAllocStatusReceived[sourceAddress]);
}