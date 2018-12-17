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
    return 0;
}

int PmmModuleDataLog::saveOwnDataLog()
{
    if (!mIsLocked)
        updateLogInfoCombinedPayload();

    // Build the 
    for (unsigned actualVar = 0, mGroupTempLength = 0; actualVar < mNumberVariables; actualVar++)
        memcpy(mGroupTempData + mPacketStruct.payloadLength, mVariableAdrsArray[actualVar], mVariableSizeArray[actualVar]);


    return saveDataLog(mGroupTempData, dirRelativePath[], mAllocStatusSelfDataLog)
}

int PmmModuleDataLog::saveReceivedDataLog(uint8_t groupData[], uint8_t groupLength, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession)
{
    return 0;
}