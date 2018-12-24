/* PmmModuleDataLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */


#include <crc.h>

#include "pmmConsts.h"
#include "pmmModules/dataLog/dataLog.h"

#include "pmmTelemetry/pmmTelemetry.h"
#include "pmmSd/pmmSd.h"


// These are important strings, which both the transmitter and the receiver must have in commom. The other variables strings
// not listed here can be freely changed.
const PROGMEM char PMM_DATA_LOG_PACKAGE_ID_STRING[]     = "packageID";
const PROGMEM char PMM_DATA_LOG_PACKAGE_TIME_STRING[]   = "packageTime(ms)";

const PROGMEM char PMM_DATA_LOG_ALTITUDE_STRING[]       = "altitude(m)";
const PROGMEM char PMM_DATA_LOG_GPS_LATITUDE_STRING[]   = "gpsLatitude";
const PROGMEM char PMM_DATA_LOG_GPS_LONGITUDE_STRING[]  = "gpsLongitude";



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



int PmmModuleDataLog::includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress)
{
    if (mIsLocked)
    {
        PMM_DEBUG_ADV_PRINT("Failed to add the variable \"")
        PMM_DEBUG_PRINT    (variableName)
        PMM_DEBUG_PRINTLN  ("\". DataLog is already locked.")
        return 1;
    }

    uint8_t varSize = variableTypeToVariableSize(variableType);

    if (mNumberVariables >= MODULE_DATA_LOG_MAX_VARIABLES)
    {
        PMM_DEBUG_ADV_PRINT("Failed to add the variable \"")
        PMM_DEBUG_PRINT    (variableName)
        PMM_DEBUG_PRINTLN  ("\". Exceeds the maximum number of variables in the DataLog.")
        return 2;
    }

    if ((mGroupLength + varSize) >= PORT_DATA_LOG_MAX_PAYLOAD_LENGTH)
    {
        PMM_DEBUG_ADV_PRINT("Failed to add the variable \"")
        PMM_DEBUG_PRINT(variableName);
        PMM_DEBUG_PRINT("\". Exceeds the maximum payload length (tried to be ");
        PMM_DEBUG_PRINT(mGroupLength + varSize);
        PMM_DEBUG_PRINT(", maximum is ");
        PMM_DEBUG_PRINT(PORT_DATA_LOG_MAX_PAYLOAD_LENGTH);
        PMM_DEBUG_PRINTLN(".");
        return 3;
    }

    mVariableNameArray[mNumberVariables] = (char*) variableName; // Typecast from (const char*) to (char*)
    mVariableTypeArray[mNumberVariables] = variableType;
    mVariableSizeArray[mNumberVariables] = varSize;
    mVariableAdrsArray[mNumberVariables] = (uint8_t*) variableAddress;
    mNumberVariables++;
    mGroupLength    += varSize;

    return 0;
}



int PmmModuleDataLog::includeArrayInPackage(const char **variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize)
{
    if (!variableName)
        return 1;
    if (!arrayAddress)
        return 2;

    uint8_t counter;
    for (counter = 0; counter < arraySize; counter++)
        includeVariableInPackage(*variableName++, arrayType, (uint8_t*) arrayAddress + (variableTypeToVariableSize(arrayType) * counter));

    return 0;
}



int PmmModuleDataLog::addBasicInfo(uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{
    if (!packageIdPtr)
        return 1;
    if (!packageTimeMsPtr)
        return 2;

    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_ID_STRING,   MODULE_DATA_LOG_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_TIME_STRING, MODULE_DATA_LOG_TYPE_UINT32, packageTimeMsPtr);
    return 0;
}



int PmmModuleDataLog::addAccelerometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"accelerometerX(g)", "accelerometerY(g)", "accelerometerZ(g)"};
    return includeArrayInPackage(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}

int PmmModuleDataLog::addGyroscope(void* array)
{
    const PROGMEM char* arrayString[3] = {"gyroscopeX(degree/s)", "gyroscopeY(degree/s)", "gyroscopeZ(degree/s)"};
    return includeArrayInPackage(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}

int PmmModuleDataLog::addMpuTemperature(void* mpuTemperature)
{
    const PROGMEM char* mpuTemperatureString = "mpuTemperature(C)";
    return includeVariableInPackage(mpuTemperatureString, MODULE_DATA_LOG_TYPE_FLOAT, mpuTemperature);
}

int PmmModuleDataLog::addMagnetometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"magnetometerX(uT)", "magnetometerY(uT)", "magnetometerZ(uT)"};
    return includeArrayInPackage(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}

int PmmModuleDataLog::addBarometer(void* barometer)
{
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";
    return includeVariableInPackage(barometerPressureString, MODULE_DATA_LOG_TYPE_FLOAT, barometer);
}

int PmmModuleDataLog::addAltitudeBarometer(void* altitudePressure)
{
    return includeVariableInPackage(PMM_DATA_LOG_ALTITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, altitudePressure);
}

int PmmModuleDataLog::addBarometerTemperature(void* barometerTempPtr)
{
    const PROGMEM char* barometerTempString = "barometerTemperature(C)";
    return includeVariableInPackage(barometerTempString, MODULE_DATA_LOG_TYPE_FLOAT, barometerTempPtr);
}



int PmmModuleDataLog::addImu(pmmImuStructType *pmmImuStructPtr)
{
    int returnVal;

    if ((returnVal = addAccelerometer(pmmImuStructPtr->accelerometerArray))) return returnVal;
    if ((returnVal = addGyroscope    (pmmImuStructPtr->gyroscopeArray))) return returnVal;
    if ((returnVal = addMpuTemperature(&pmmImuStructPtr->mpuTemperature))) return returnVal;

    if ((returnVal = addMagnetometer (pmmImuStructPtr->magnetometerArray))) return returnVal;

    if ((returnVal = addBarometer           (&pmmImuStructPtr->pressure))) return returnVal;
    if ((returnVal = addAltitudeBarometer   (&pmmImuStructPtr->altitudePressure))) return returnVal;
    if ((returnVal = addBarometerTemperature(&pmmImuStructPtr->barometerTemperature))) return returnVal;
    
    return 0;
}



int PmmModuleDataLog::addGps(pmmGpsStructType* pmmGpsStruct)
{
    int returnVal;

    #ifdef GPS_FIX_LOCATION
        if ((returnVal = includeVariableInPackage(PMM_DATA_LOG_GPS_LATITUDE_STRING,  MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->latitude))) return returnVal;;
        if ((returnVal = includeVariableInPackage(PMM_DATA_LOG_GPS_LONGITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->longitude))) return returnVal;;
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char* gpsAltitudeString = "gpsAltitude(m)";
        if ((returnVal = includeVariableInPackage(gpsAltitudeString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->altitude))) return returnVal;;
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char* gpsSatellitesString = "gpsSatellites";
        if ((returnVal = includeVariableInPackage(gpsSatellitesString, MODULE_DATA_LOG_TYPE_UINT8, &pmmGpsStruct->satellites))) return returnVal;;
    #endif
    
    #ifdef GPS_FIX_SPEED
        const PROGMEM char* gpsHorizontalSpeedString = "gpsHorSpeed(m/s)";
        const PROGMEM char* gpsNorthSpeedString = "gpsNorthSpeed(m/s)";
        const PROGMEM char* gpsEastSpeedString = "gpsEastSpeed(m/s)";
        const PROGMEM char* gpsHeadingDegreeString = "gpsHeadingDegree";
        if ((returnVal = includeVariableInPackage(gpsHorizontalSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->horizontalSpeed))) return returnVal;;
        if ((returnVal = includeVariableInPackage(gpsNorthSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->northSpeed))) return returnVal;;
        if ((returnVal = includeVariableInPackage(gpsEastSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->eastSpeed))) return returnVal;;
        if ((returnVal = includeVariableInPackage(gpsHeadingDegreeString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->headingDegree))) return returnVal;;

        #ifdef GPS_FIX_ALTITUDE
            const PROGMEM char* gpsUpSpeedString = "gpsSpeedUp(m/s)";
            if ((returnVal = includeVariableInPackage(gpsUpSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->upSpeed))) return returnVal;;
        #endif
    #endif

    return 0;
}



int PmmModuleDataLog::addCustomVariable(const char* variableName, uint8_t variableType, void* variableAddress)
{
    return includeVariableInPackage(variableName, variableType, variableAddress);
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
