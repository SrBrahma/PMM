#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/dataLog/dataLogInfo/logInfo.h"
#include "pmmModules/dataLog/dataLogGroupCore.h"

void PmmModuleDataLogGroupCore::buildLogInfoArray()
{
    unsigned variableCounter;
    unsigned stringLength;              // The length withou null char!

    mLogInfoContentArrayLength = 0;  // Zero the length of the array.


    // 1) Add the "Number of variables"
    mLogInfoContentArray[0] = mNumberVariables;
    mLogInfoContentArrayLength++;


    // 2) Add the "Variable types". Adds them in groups of two, as the types are always <= 4 bits, the group makes 8 bits.
    for (variableCounter = 0; variableCounter < mNumberVariables; variableCounter += 2)
    {
        if (mNumberVariables - variableCounter >= 2)
            mLogInfoContentArray[mLogInfoContentArrayLength] = (mVariableTypeArray[variableCounter] << 4) | (mVariableTypeArray[variableCounter + 1]);
        else // Else, the number is odd (rest of division by 2 is 1)
            mLogInfoContentArray[mLogInfoContentArrayLength] = (mVariableTypeArray[variableCounter] << 4);
        mLogInfoContentArrayLength++;
    }

    // 3) Add the Variable strings
    for (variableCounter = 0; variableCounter < mNumberVariables; variableCounter ++)
    {
        // As I couldn't find a way to use strnlen, made it!
        // Again, the stringLength doesn't include the '\0'. The '\0' is manually added in the next lines.
        for (stringLength = 0;
             ((stringLength < (MODULE_DATA_LOG_MAX_STRING_LENGTH - 1)) && mVariableNameArray[variableCounter][stringLength]); // - 1 as the MAX_STRING_LENGTH includes the '\0'.
             stringLength++); 
            
        memcpy(mLogInfoContentArray + mLogInfoContentArrayLength, mVariableNameArray[variableCounter], stringLength);
        mLogInfoContentArrayLength += stringLength;
        mLogInfoContentArray[mLogInfoContentArrayLength] = '\0'; // Manually write the null terminating char, in case the string was broken.
        mLogInfoContentArrayLength ++;
    }

    // 4) Calculate the total number of packets.
    mDataLogInfoPackets = ceil(mLogInfoContentArrayLength / (float) PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);

    // 5) Get the path to this DataLog
    char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];
    getDataLogDirectory(tempFilename, PMM_SD_FILENAME_MAX_LENGTH, mDataLogGroupId, mGroupLength);
    mPmmSdPtr->getSelfDirectory(mDataLogSelfDirPath, PMM_SD_FILENAME_MAX_LENGTH, tempFilename);

    // 6) Save the DataLogInfo.
    saveOwnDataLogInfo();

    // 7) Set this to store the DataLog
    mAllocStatusSelfDataLog.groupLength = mGroupLength;

    // 8) Finally, lock this DataLog!
    mIsGroupLocked = 1;
}