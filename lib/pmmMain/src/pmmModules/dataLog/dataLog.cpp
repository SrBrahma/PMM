/* PmmModuleDataLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */


#include "pmmConsts.h"

#include "pmmTelemetry/pmmTelemetry.h"

#include "pmmModules/dataLog/dataLog.h"



PmmModuleDataLog::PmmModuleDataLog()
{
}



int PmmModuleDataLog::init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{

    mPmmTelemetry       = pmmTelemetry;
    mPmmSd              = pmmSd;
    
    mIsLocked           = 0;
    mGroupLength        = 0;
    mNumberVariables    = 0;
    mDataLogInfoPackets = 0;

    mSystemSession      = systemSession;

    // These variables are always added to the package.
    addBasicInfo(packageIdPtr, packageTimeMsPtr);

    return 0;
}


int PmmModuleDataLog::update()
{
    // We will only add to the telemetry queue this packet if there are no packets to be sent ahead of this one! We prefer
    // to send updated logs! Not old ones!
    // And I might change it on the future!
    // However, packets from other modules added to the queue with a higher priority still may be added, and will be sent first.
    // On the future, as always, I may make it better, maybe replacing the old packet on the queue with a new one.
    if (mPmmTelemetry->getTotalPacketsRemainingOnQueue() == 0)
        sendDataLog();

    return 0;
}



int PmmModuleDataLog::setSystemMode(pmmSystemState systemMode)
{
    mSystemSession = systemMode;

    return 0;
}



uint8_t PmmModuleDataLog::variableTypeToVariableSize(uint8_t variableType)
{
    switch (variableType)
    {
        case MODULE_DATA_LOG_TYPE_UINT8:
            return 1;
        case MODULE_DATA_LOG_TYPE_INT8:
            return 1;
        case MODULE_DATA_LOG_TYPE_UINT16:
            return 2;
        case MODULE_DATA_LOG_TYPE_INT16:
            return 2;
        case MODULE_DATA_LOG_TYPE_UINT32:
            return 4;
        case MODULE_DATA_LOG_TYPE_INT32:
            return 4;
        case MODULE_DATA_LOG_TYPE_FLOAT:
            return 4;
        case MODULE_DATA_LOG_TYPE_UINT64:
            return 8;
        case MODULE_DATA_LOG_TYPE_INT64:
            return 8;
        case MODULE_DATA_LOG_TYPE_DOUBLE:
            return 8;
        default:    // Maybe will avoid internal crashes?
            PMM_DEBUG_ADV_PRINT("Invalid variable type to size!");
            return 1;
    }
}



/* Getters! -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
uint8_t PmmModuleDataLog::getNumberOfVariables()
{
    return mNumberVariables;
}



const char** PmmModuleDataLog::getVariableNameArray()    { return (const char**) mVariableNameArray;}
uint8_t*     PmmModuleDataLog::getVariableTypeArray()    { return mVariableTypeArray;}
uint8_t*     PmmModuleDataLog::getVariableSizeArray()    { return mVariableSizeArray;}
uint8_t**    PmmModuleDataLog::getVariableAdrsArray()    { return mVariableAdrsArray;}







// Debug! -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void PmmModuleDataLog::debugPrintLogHeader()
{
    char buffer[2048] = {'\0'}; // No static needed, as it is called usually only once.

    for (unsigned variableIndex = 0; variableIndex < mNumberVariables; variableIndex ++)
    {
        if (variableIndex > 0)
            snprintf(buffer, 2048, "%s ", buffer);
        snprintf(buffer, 2048, "%s[%u) %s", buffer, variableIndex, mVariableNameArray[variableIndex]);
        snprintf(buffer, 2048, "%s]", buffer);
    }

    Serial.println(buffer);
}


// Float variables are printed with a maximum of 3 decimal digits. You may change it if you like.
void PmmModuleDataLog::debugPrintLogContent()
{
    static char buffer[512]; // Static for optimization
    buffer[0] = {'\0'};      // As the above is static, we need to reset the first char so snprintf will work properly.

    for (unsigned variableIndex = 0; variableIndex < mNumberVariables; variableIndex ++)
    {
        if (variableIndex > 0)
            snprintf(buffer, 512, "%s ", buffer);

        snprintf(buffer, 512, "%s[%u) ", buffer, variableIndex);

        switch(mVariableTypeArray[variableIndex])
        {
            case MODULE_DATA_LOG_TYPE_FLOAT: // first as it is more common
                snprintf(buffer, 512, "%s%.3f", buffer, *(float*)    (mVariableAdrsArray[variableIndex])); // https://stackoverflow.com/a/30658980/10247962
                break;
            case MODULE_DATA_LOG_TYPE_UINT32:
                snprintf(buffer, 512, "%s%lu",  buffer, *(uint32_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT32:
                snprintf(buffer, 512, "%s%li",  buffer, *(int32_t*)  (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT8:
                snprintf(buffer, 512, "%s%u",   buffer, *(uint8_t*)  (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT8:
                snprintf(buffer, 512, "%s%i",   buffer, *(int8_t*)   (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT16:
                snprintf(buffer, 512, "%s%u",   buffer, *(uint16_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT16:
                snprintf(buffer, 512, "%s%i",   buffer, *(int16_t*)  (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT64:
                snprintf(buffer, 512, "%s%llu", buffer, *(uint64_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT64:
                snprintf(buffer, 512, "%s%lli", buffer, *(int64_t*)  (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_DOUBLE:
                snprintf(buffer, 512, "%s%.3f", buffer, *(double*)   (mVariableAdrsArray[variableIndex]));
                break;
            default:    // If none above,
                snprintf(buffer, 512, "%s%s",   buffer, ">TYPE ERROR HERE!<");
                break;
        } // switch end
        snprintf(buffer, 512, "%s]", buffer);
    } // for loop end
    Serial.println(buffer);
} // end of function debugPrintLogContent()
