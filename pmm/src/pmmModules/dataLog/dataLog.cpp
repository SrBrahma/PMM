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



int PmmModuleDataLog::init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t* systemSessionPtr, uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{

    mPmmTelemetry = pmmTelemetry;
    mPmmSd = pmmSd;
    
    mPackageLogSizeInBytes = 0;
    mLogNumberOfVariables = 0;
    mPackageLogInfoNumberOfPackets = 0; // For receptor.

    mSystemSessionPtr = systemSessionPtr;

    // These variables are always added to the package.
    addPackageBasicInfo(packageIdPtr, packageTimeMsPtr);

    return 0;
}



uint8_t PmmModuleDataLog::variableTypeToVariableSize(uint8_t variableType)
{
    switch (variableType)
    {
        case PMM_EXTENSION_DATA_LOG_TYPE_UINT8:
            return 1;
        case PMM_EXTENSION_DATA_LOG_TYPE_INT8:
            return 1;
        case PMM_EXTENSION_DATA_LOG_TYPE_UINT16:
            return 2;
        case PMM_EXTENSION_DATA_LOG_TYPE_INT16:
            return 2;
        case PMM_EXTENSION_DATA_LOG_TYPE_UINT32:
            return 4;
        case PMM_EXTENSION_DATA_LOG_TYPE_INT32:
            return 4;
        case PMM_EXTENSION_DATA_LOG_TYPE_FLOAT:
            return 4;
        case PMM_EXTENSION_DATA_LOG_TYPE_UINT64:
            return 8;
        case PMM_EXTENSION_DATA_LOG_TYPE_INT64:
            return 8;
        case PMM_EXTENSION_DATA_LOG_TYPE_DOUBLE:
            return 8;
        default:    // Maybe will avoid internal crashes?
            PMM_DEBUG_PRINT("PmmPort #1: Invalid variable type to size!");
            return 1;
    }
}



void PmmModuleDataLog::includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress)
{
    uint8_t varSize = variableTypeToVariableSize(variableType);
    if (mLogNumberOfVariables >= PMM_EXTENSION_DATA_LOG_MAX_VARIABLES)
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("PmmPort #2: Failed to add the variable \"");
            Serial.print(variableName);
            Serial.print("\". Exceeds the maximum number of variables in the Package Log.\n");
        #endif
        return;
    }
    if ((mPackageLogSizeInBytes + varSize) >= PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH)
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("PmmPort #3: Failed to add the variable \"");
            Serial.print(variableName);
            Serial.print("\". Exceeds the maximum payload length (tried to be ");
            Serial.print((mPackageLogSizeInBytes + varSize));
            Serial.print(", maximum is ");
            Serial.print(PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH);
            Serial.print(".\n");
        #endif
        return;
    }

    mVariableNameArray[mLogNumberOfVariables] = (char*) variableName; // Typecast from (const char*) to (char*)
    mVariableTypeArray[mLogNumberOfVariables] = variableType;
    mVariableSizeArray[mLogNumberOfVariables] = varSize;
    mVariableAddressArray[mLogNumberOfVariables] = (uint8_t*) variableAddress;
    mLogNumberOfVariables ++;
    mPackageLogSizeInBytes += varSize;

    updateLogInfoCombinedPayload();
    updateLogInfoInTelemetryFormat();
}

void PmmModuleDataLog::includeArrayInPackage(const char **variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize)
{
    uint8_t counter;
    for (counter = 0; counter < arraySize; counter++)
        includeVariableInPackage(*variableName++, arrayType, (uint8_t*) arrayAddress + (variableTypeToVariableSize(arrayType) * counter));
}



void PmmModuleDataLog::addPackageBasicInfo(uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{
    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_ID_STRING,           PMM_EXTENSION_DATA_LOG_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_TIME_STRING,         PMM_EXTENSION_DATA_LOG_TYPE_UINT32, packageTimeMsPtr);
}



void PmmModuleDataLog::addMagnetometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"magnetometerX(uT)", "magnetometerY(uT)", "magnetometerZ(uT)"};
    includeArrayInPackage(arrayString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, array, 3);
}



void PmmModuleDataLog::addGyroscope(void* array)
{
    const PROGMEM char* arrayString[3] = {"gyroscopeX(degree/s)", "gyroscopeY(degree/s)", "gyroscopeZ(degree/s)"};
    includeArrayInPackage(arrayString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, array, 3);
}



void PmmModuleDataLog::addAccelerometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"accelerometerX(g)", "accelerometerY(g)", "accelerometerZ(g)"};
    includeArrayInPackage(arrayString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, array, 3);
}



void PmmModuleDataLog::addBarometer(void* barometer)
{
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";
    includeVariableInPackage(barometerPressureString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, barometer);
}



void PmmModuleDataLog::addAltitudeBarometer(void* altitudePressure)
{
    includeVariableInPackage(PMM_DATA_LOG_ALTITUDE_STRING, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, altitudePressure);
}



void PmmModuleDataLog::addThermometer(void* thermometerPtr)
{
    const PROGMEM char* thermometerString = "temperature(C)";
    includeVariableInPackage(thermometerString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, thermometerPtr);
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
        includeVariableInPackage(PMM_DATA_LOG_GPS_LATITUDE_STRING,  PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->latitude));
        includeVariableInPackage(PMM_DATA_LOG_GPS_LONGITUDE_STRING, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->longitude));
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char* gpsAltitudeString = "gpsAltitude(m)";
        includeVariableInPackage(gpsAltitudeString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->altitude));
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char* gpsSatellitesString = "gpsSatellites";
        includeVariableInPackage(gpsSatellitesString, PMM_EXTENSION_DATA_LOG_TYPE_UINT8, &(pmmGpsStruct->satellites));
    #endif
    /*
    #ifdef GPS_FIX_SPEED
        const PROGMEM char* gpsHorizontalSpeedString = "gpsHorSpeed(m/s)";
        const PROGMEM char* gpsNorthSpeedString = "gpsNorthSpeed(m/s)";
        const PROGMEM char* gpsEastSpeedString = "gpsEastSpeed(m/s)";
        const PROGMEM char* gpsHeadingDegreeString = "gpsHeadingDegree";
        includeVariableInPackage(gpsHorizontalSpeedString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->horizontalSpeed));
        includeVariableInPackage(gpsNorthSpeedString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->northSpeed));
        includeVariableInPackage(gpsEastSpeedString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->eastSpeed));
        includeVariableInPackage(gpsHeadingDegreeString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &(pmmGpsStruct->headingDegree));

        #ifdef GPS_FIX_ALTITUDE
            const PROGMEM char* gpsUpSpeedString = "gpsSpeedUp(m/s)";
            includeVariableInPackage(gpsUpSpeedString, PMM_EXTENSION_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->upSpeed);
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
    return mLogNumberOfVariables;
}



uint8_t PmmModuleDataLog::getPackageLogSizeInBytes()
{
    return mPackageLogSizeInBytes;
}



const char** PmmModuleDataLog::getVariableNameArray()    { return (const char**) mVariableNameArray;}
uint8_t*     PmmModuleDataLog::getVariableTypeArray()    { return mVariableTypeArray;}
uint8_t*     PmmModuleDataLog::getVariableSizeArray()    { return mVariableSizeArray;}
uint8_t**    PmmModuleDataLog::getVariableAddressArray() { return mVariableAddressArray;}







/* Debug! -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#if PMM_DEBUG_SERIAL
// Note for the 2 functions below:
// There are faster ways to print the debugs, but since it isn't something that is going to be used frequently,
// I (HB :) ) will spend my precious time on other stuffs)

void PmmModuleDataLog::debugPrintLogHeader()
{
    unsigned variableIndex;
    char buffer[512] = {0}; // No static needed, as it is called usually only once.

    // For adding the first variable header to the print
    if (mLogNumberOfVariables > 0)
        snprintf(buffer, 512, "%s", mVariableNameArray[0]);

    for (variableIndex = 1; variableIndex < mLogNumberOfVariables; variableIndex ++)
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
    for (variableIndex = 0; variableIndex < mLogNumberOfVariables; variableIndex ++)
    {
        switch(mVariableTypeArray[variableIndex])
        {
            case PMM_EXTENSION_DATA_LOG_TYPE_FLOAT: // first as it is more common
                snprintf(buffer, 512, "%s%f, ", buffer, *(float*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_UINT32:
                snprintf(buffer, 512, "%s%lu, ", buffer, *(uint32_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_INT32:
                snprintf(buffer, 512, "%s%li, ", buffer, *(int32_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_UINT8:
                snprintf(buffer, 512, "%s%u, ", buffer, *(uint8_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_INT8:
                snprintf(buffer, 512, "%s%i, ", buffer, *(int8_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_UINT16:
                snprintf(buffer, 512, "%s%u, ", buffer, *(uint16_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_INT16:
                snprintf(buffer, 512, "%s%i, ", buffer, *(int16_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_UINT64:
                snprintf(buffer, 512, "%s%llu, ", buffer, *(uint64_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_INT64:
                snprintf(buffer, 512, "%s%lli, ", buffer, *(int64_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_EXTENSION_DATA_LOG_TYPE_DOUBLE:
                snprintf(buffer, 512, "%s%f, ", buffer, *(double*) (mVariableAddressArray[variableIndex]));
                break;
            default:    // If none above,
                snprintf(buffer, 512, "%s%s, ", buffer, ">TYPE ERROR HERE!<");
                break;
        } // switch end
    } // for loop end
    Serial.println(buffer);
} // end of function debugPrintLogContent()



#endif
