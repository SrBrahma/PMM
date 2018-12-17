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
const PROGMEM char PMM_DATA_LOG_PACKAGE_ID_STRING[]           = "packageID";
const PROGMEM char PMM_DATA_LOG_PACKAGE_TIME_STRING[]         = "packageTime(ms)";

const PROGMEM char PMM_DATA_LOG_ALTITUDE_STRING[]             = "altitude(m)";
const PROGMEM char PMM_DATA_LOG_GPS_LATITUDE_STRING[]         = "gpsLongitude";
const PROGMEM char PMM_DATA_LOG_GPS_LONGITUDE_STRING[]        = "gpsLatitude";



PmmModuleDataLog::PmmModuleDataLog()
{
}



int PmmModuleDataLog::init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{

    mPmmTelemetry       = pmmTelemetry;
    mPmmSd              = pmmSd;
    
    mIsLocked           = 0;
    mDataLogSize        = 0;
    mNumberVariables    = 0;
    mDataLogInfoPackets = 0;

    mSystemSession      = systemSession;

    // These variables are always added to the package.
    addPackageBasicInfo(packageIdPtr, packageTimeMsPtr);

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
        PMM_DEBUG_PRINT(variableName)
        PMM_DEBUG_PRINTLN("\". DataLog is already locked.")
        return 1;
    }

    uint8_t varSize = variableTypeToVariableSize(variableType);

    if (mNumberVariables >= MODULE_DATA_LOG_MAX_VARIABLES)
    {
        PMM_DEBUG_ADV_PRINT("Failed to add the variable \"")
        PMM_DEBUG_PRINT(variableName)
        PMM_DEBUG_PRINTLN("\". Exceeds the maximum number of variables in the DataLog.")
        return 2;
    }

    if ((mDataLogSize + varSize) >= PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH)
    {
        PMM_DEBUG_ADV_PRINT("Failed to add the variable \"")
        PMM_DEBUG_PRINT(variableName);
        PMM_DEBUG_PRINT("\". Exceeds the maximum payload length (tried to be ");
        PMM_DEBUG_PRINT(mDataLogSize + varSize);
        PMM_DEBUG_PRINT(", maximum is ");
        PMM_DEBUG_PRINT(PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH);
        PMM_DEBUG_PRINTLN(".");
        return 3;
    }

    mVariableNameArray[mNumberVariables] = (char*) variableName; // Typecast from (const char*) to (char*)
    mVariableTypeArray[mNumberVariables] = variableType;
    mVariableSizeArray[mNumberVariables] = varSize;
    mVariableAdrsArray[mNumberVariables] = (uint8_t*) variableAddress;
    mNumberVariables++;
    mDataLogSize    += varSize;

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



int PmmModuleDataLog::addPackageBasicInfo(uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{
    if (!packageIdPtr)
        return 1;
    if (!packageTimeMsPtr)
        return 2;

    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_ID_STRING,   MODULE_DATA_LOG_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_TIME_STRING, MODULE_DATA_LOG_TYPE_UINT32, packageTimeMsPtr);
    return 0;
}



void PmmModuleDataLog::addMagnetometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"magnetometerX(uT)", "magnetometerY(uT)", "magnetometerZ(uT)"};
    includeArrayInPackage(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}



void PmmModuleDataLog::addGyroscope(void* array)
{
    const PROGMEM char* arrayString[3] = {"gyroscopeX(degree/s)", "gyroscopeY(degree/s)", "gyroscopeZ(degree/s)"};
    includeArrayInPackage(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}



void PmmModuleDataLog::addAccelerometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"accelerometerX(g)", "accelerometerY(g)", "accelerometerZ(g)"};
    includeArrayInPackage(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}



void PmmModuleDataLog::addBarometer(void* barometer)
{
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";
    includeVariableInPackage(barometerPressureString, MODULE_DATA_LOG_TYPE_FLOAT, barometer);
}



void PmmModuleDataLog::addAltitudeBarometer(void* altitudePressure)
{
    includeVariableInPackage(PMM_DATA_LOG_ALTITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, altitudePressure);
}



void PmmModuleDataLog::addThermometer(void* thermometerPtr)
{
    const PROGMEM char* thermometerString = "temperature(C)";
    includeVariableInPackage(thermometerString, MODULE_DATA_LOG_TYPE_FLOAT, thermometerPtr);
}



void PmmModuleDataLog::addImu(pmmImuStructType *pmmImuStructPtr)
{
    addAccelerometer(pmmImuStructPtr->accelerometerArray);
    addGyroscope(pmmImuStructPtr->gyroscopeArray);
    addMagnetometer(pmmImuStructPtr->magnetometerArray);

    addBarometer(&pmmImuStructPtr->pressure);
    addAltitudeBarometer(&pmmImuStructPtr->altitudePressure);
    addThermometer(&pmmImuStructPtr->temperature);
}



void PmmModuleDataLog::addGps(pmmGpsStructType* pmmGpsStruct)
{
    #ifdef GPS_FIX_LOCATION
        includeVariableInPackage(PMM_DATA_LOG_GPS_LATITUDE_STRING,  MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->latitude));
        includeVariableInPackage(PMM_DATA_LOG_GPS_LONGITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->longitude));
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char* gpsAltitudeString = "gpsAltitude(m)";
        includeVariableInPackage(gpsAltitudeString, MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->altitude));
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char* gpsSatellitesString = "gpsSatellites";
        includeVariableInPackage(gpsSatellitesString, MODULE_DATA_LOG_TYPE_UINT8, &(pmmGpsStruct->satellites));
    #endif
    /*
    #ifdef GPS_FIX_SPEED
        const PROGMEM char* gpsHorizontalSpeedString = "gpsHorSpeed(m/s)";
        const PROGMEM char* gpsNorthSpeedString = "gpsNorthSpeed(m/s)";
        const PROGMEM char* gpsEastSpeedString = "gpsEastSpeed(m/s)";
        const PROGMEM char* gpsHeadingDegreeString = "gpsHeadingDegree";
        includeVariableInPackage(gpsHorizontalSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->horizontalSpeed));
        includeVariableInPackage(gpsNorthSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->northSpeed));
        includeVariableInPackage(gpsEastSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->eastSpeed));
        includeVariableInPackage(gpsHeadingDegreeString, MODULE_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->headingDegree));

        #ifdef GPS_FIX_ALTITUDE
            const PROGMEM char* gpsUpSpeedString = "gpsSpeedUp(m/s)";
            includeVariableInPackage(gpsUpSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->upSpeed);
        #endif
    #endif*/
}



void PmmModuleDataLog::addCustomVariable(const char* variableName, uint8_t variableType, void* variableAddress)
{
    includeVariableInPackage(variableName, variableType, variableAddress);
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
    unsigned variableIndex;
    char buffer[512] = {0}; // No static needed, as it is called usually only once.

    // For adding the first variable header to the print
    if (mNumberVariables > 0)
        snprintf(buffer, 512, "%s", mVariableNameArray[0]);

    for (variableIndex = 1; variableIndex < mNumberVariables; variableIndex ++)
    {
        snprintf(buffer, 512, "%s | %s", buffer, mVariableNameArray[variableIndex]);
    }
    Serial.println(buffer);
}

void PmmModuleDataLog::debugPrintLogContent()
{
    unsigned variableIndex;
    static char buffer[512]; // Static for optimization
    buffer[0] = '\0'; // for the snprintf
    for (variableIndex = 0; variableIndex < mNumberVariables; variableIndex ++)
    {
        switch(mVariableTypeArray[variableIndex])
        {
            case MODULE_DATA_LOG_TYPE_FLOAT: // first as it is more common
                snprintf(buffer, 512, "%s%f, ", buffer, *(float*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT32:
                snprintf(buffer, 512, "%s%lu, ", buffer, *(uint32_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT32:
                snprintf(buffer, 512, "%s%li, ", buffer, *(int32_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT8:
                snprintf(buffer, 512, "%s%u, ", buffer, *(uint8_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT8:
                snprintf(buffer, 512, "%s%i, ", buffer, *(int8_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT16:
                snprintf(buffer, 512, "%s%u, ", buffer, *(uint16_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT16:
                snprintf(buffer, 512, "%s%i, ", buffer, *(int16_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_UINT64:
                snprintf(buffer, 512, "%s%llu, ", buffer, *(uint64_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_INT64:
                snprintf(buffer, 512, "%s%lli, ", buffer, *(int64_t*) (mVariableAdrsArray[variableIndex]));
                break;
            case MODULE_DATA_LOG_TYPE_DOUBLE:
                snprintf(buffer, 512, "%s%f, ", buffer, *(double*) (mVariableAdrsArray[variableIndex]));
                break;
            default:    // If none above,
                snprintf(buffer, 512, "%s%s, ",   buffer, ">TYPE ERROR HERE!<");
                break;
        } // switch end
    } // for loop end
    Serial.println(buffer);
} // end of function debugPrintLogContent()
