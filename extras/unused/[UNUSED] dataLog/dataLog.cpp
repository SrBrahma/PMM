/* PmmModuleDataLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */


#include "pmmConsts.h"

#include "pmmTelemetry/telemetry.h"

#include "pmmModules/dataLog/dataLog.h"



PmmModuleDataLog::PmmModuleDataLog() {}



// For now, there will be only one group.
int PmmModuleDataLog::init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* mainLoopCounterPtr, uint32_t* timeMillisPtr)
{
    mUpdateModeReadyCounter   = mUpdateModeDeployedCounter = 0;
    mUpdateDataLogInfoCounter = 0;

    mPmmTelemetryPtr          = pmmTelemetry;
    mPmmSdPtr                 = pmmSd;
    mPmmSdSafeLogPtr          = mPmmSdPtr->getSafeLog();

    mSystemSession            = systemSession;

    mDataLogGroupCore.init(mPmmTelemetryPtr, mPmmSdPtr, mPmmSdSafeLogPtr, mSystemSession, 0);
    mDataLogGroupCore.addBasicInfo(mainLoopCounterPtr, timeMillisPtr);

    return 0;
}

PmmModuleDataLogGroupCore* PmmModuleDataLog::getDataLogGroupCore(uint8_t dataLogGroupId)
{
    return &mDataLogGroupCore;
}

int PmmModuleDataLog::update()
{
    if (!mDataLogGroupCore.getIsGroupLocked())
        return 0;

    // switch (mSystemMode)
    // {
    //     case MODE_SLEEP:
    //         return 0;

    //     case MODE_READY:
    //         if (mUpdateModeReadyCounter < 5)
    //         {
    //             if (mPmmTelemetryPtr->getTotalPacketsRemainingOnQueue() == 0)
    //                 if(!mDataLogGroupCore.sendDataLogInfo(mUpdateDataLogInfoCounter++))
    //                     mUpdateModeReadyCounter++;
    //         }
    //         else
    //         {
    //             if (mPmmTelemetryPtr->getTotalPacketsRemainingOnQueue() == 0)
    //                 if(!mDataLogGroupCore.sendDataLog())
    //                     mUpdateModeReadyCounter = 0;
    //         }
    //         break;

    //     case MODE_DEPLOYED:
    //     case MODE_FINISHED:
    //         if (mUpdateModeDeployedCounter < 1) // Send lots of DataLog before the DataLogInfo!
    //         {
    //             // This 'if' is to always send the newest dataLog package. However, some other package may still be sent first if added to the queue with a
    //             // higher priority.
    //             if (mPmmTelemetryPtr->getTotalPacketsRemainingOnQueue() == 0)
    //                 if (!mDataLogGroupCore.sendDataLog())
    //                     mUpdateModeDeployedCounter++;   // Only increase in successful sents.
    //         }
    //         else // Every once a while send a DataLogInfo packet!
    //         {
    //             mDataLogGroupCore.sendDataLogInfo(mUpdateDataLogInfoCounter++);
    //             mUpdateModeDeployedCounter = 0;
    //         }

    //         break;
    // }   // End of switch

    // if (mUpdateDataLogInfoCounter >= mDataLogGroupCore.getDataLogInfoPackets())
    //     mUpdateDataLogInfoCounter = 0;

    // mDataLogGroupCore.saveOwnDataLog();

    return 0;
}



// Debug! -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void PmmModuleDataLog::debugPrintLogHeader()
{
    char buffer[2048] = {'\0'}; // No static needed, as it is called usually only once.

    for (unsigned variableIndex = 0; variableIndex < mDataLogGroupCore.getNumberOfVariables(); variableIndex ++)
    {
        if (variableIndex > 0)
            snprintf(buffer, 2048, "%s ", buffer);
        snprintf(buffer, 2048, "%s[%u) %s", buffer, variableIndex, mDataLogGroupCore.getVarNameArray()[variableIndex]);
        snprintf(buffer, 2048, "%s]", buffer);
    }

    Serial.println(buffer);
}


#define DATA_LOG_DEBUG_BUFFER_LEN 2048
// Float variables are printed with a maximum of 3 decimal digits. You may change it if you like.
void PmmModuleDataLog::debugPrintLogContent(bool printHeadersTogether)
{
#if PMM_DEBUG && PMM_DATA_LOG_DEBUG
    static char buffer[DATA_LOG_DEBUG_BUFFER_LEN]; // Static for optimization

    strcpy(buffer, "<");    // Zero the string, with a symbolic header.

    for (unsigned variableIndex = 0; variableIndex < mDataLogGroupCore.getNumberOfVariables(); variableIndex ++)
    {
        if (variableIndex > 0)
            snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s ", buffer);

        // Print the index.
        snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s[%u) ", buffer, variableIndex);

        // Print the header.
        if (printHeadersTogether)
            snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%s: ", buffer, mDataLogGroupCore.getVarNameArray()[variableIndex]);

        switch(mDataLogGroupCore.getVarTypeArray()[variableIndex])
        {
            case TYPE_ID_FLOAT: // first as it is more common
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%.2f", buffer, *(float*)    (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_UINT32:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%lu",  buffer, *(uint32_t*) (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_INT32:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%li",  buffer, *(int32_t*)  (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_UINT8:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%u",   buffer, *(uint8_t*)  (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_INT8:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%i",   buffer, *(int8_t*)   (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_UINT16:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%u",   buffer, *(uint16_t*) (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_INT16:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%i",   buffer, *(int16_t*)  (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_UINT64:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%llu", buffer, *(uint64_t*) (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_INT64:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%lli", buffer, *(int64_t*)  (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            case TYPE_ID_DOUBLE:
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%.2f", buffer, *(double*)   (mDataLogGroupCore.getVarAdrsArray()[variableIndex]));
                break;
            default:    // If none above,
                snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s%s",   buffer, "ERROR HERE!");
                break;
        } // switch end
        snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s]", buffer);
    } // for loop end
    snprintf(buffer, DATA_LOG_DEBUG_BUFFER_LEN, "%s>\n", buffer);
    Serial.println(buffer);

    #endif
} // end of function debugPrintLogContent()
#undef DATA_LOG_DEBUG_BUFFER_LEN