#include "pmmGps/pmmGps.h"                  // For GPS struct
#include "pmmImu/pmmImu.h"                  // For IMU struct
#include "pmmModules/dataLog/dataLog.h"


// These are important strings, which both the transmitter and the receiver must have in commom. The other variables strings
// not listed here can be freely changed.
const PROGMEM char PMM_DATA_LOG_PACKAGE_ID_STRING[]         = "mainLoopCounter";
const PROGMEM char PMM_DATA_LOG_PACKAGE_TIME_STRING[]       = "mainTime(ms)";

const PROGMEM char PMM_DATA_LOG_RAW_ALTITUDE_STRING[]       = "rawAltitudeBarometer(m)";
const PROGMEM char PMM_DATA_LOG_ALTITUDE_STRING[]           = "AltitudeBarometer(m)";

const PROGMEM char PMM_DATA_LOG_GPS_LATITUDE_STRING[]       = "gpsLatitude";
const PROGMEM char PMM_DATA_LOG_GPS_LONGITUDE_STRING[]      = "gpsLongitude";



int PmmModuleDataLog::includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress)
{
    if (mIsLocked)
    {
        advPrintf("Failed to add the variable \"%s\". DataLog is already locked.\n", variableName)
        return 1;
    }

    uint8_t varSize = variableTypeToVariableSize(variableType);

    if (mNumberVariables >= MODULE_DATA_LOG_MAX_VARIABLES)
    {
        advPrintf("Failed to add the variable \"%s\". Exceeds the maximum number of variables in the DataLog.", variableName)
        return 2;
    }

    if ((mGroupLength + varSize) >= PORT_DATA_LOG_MAX_PAYLOAD_LENGTH)
    {
        advPrintf("Failed to add the variable \"%s\". Exceeds the maximum content byte size (tried to be %u, max is %u).", variableName, mGroupLength + varSize, PORT_DATA_LOG_MAX_PAYLOAD_LENGTH)
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



int PmmModuleDataLog::addBasicInfo(uint32_t* mainLoopCounterPtr, uint32_t* mainMillisPtr)
{
    if (!mainLoopCounterPtr)
        return 1;
    if (!mainMillisPtr)
        return 2;

    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_ID_STRING,   MODULE_DATA_LOG_TYPE_UINT32, mainLoopCounterPtr);
    includeVariableInPackage(PMM_DATA_LOG_PACKAGE_TIME_STRING, MODULE_DATA_LOG_TYPE_UINT32, mainMillisPtr);
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

int PmmModuleDataLog::addTemperatureMpu(void* temperatureMpu)
{
    const PROGMEM char* mpuTemperatureString = "temperatureMpu(C)";
    return includeVariableInPackage(mpuTemperatureString, MODULE_DATA_LOG_TYPE_FLOAT, temperatureMpu);
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

// Without filtering
int PmmModuleDataLog::addRawAltitudeBarometer(void* rawAltitudePressure)
{
    return includeVariableInPackage(PMM_DATA_LOG_RAW_ALTITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, rawAltitudePressure);
}

int PmmModuleDataLog::addAltitudeBarometer(void* altitude)
{
    return includeVariableInPackage(PMM_DATA_LOG_ALTITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, altitude);
}

int PmmModuleDataLog::addTemperatureBmp(void* barometerTempPtr)
{
    const PROGMEM char* barometerTempString = "temperatureBmp(C)";
    return includeVariableInPackage(barometerTempString, MODULE_DATA_LOG_TYPE_FLOAT, barometerTempPtr);
}



int PmmModuleDataLog::addImu(pmmImuStructType *pmmImuStructPtr)
{
    int returnVal;

    if ((returnVal = addAccelerometer       (pmmImuStructPtr->accelerometerArray))) return returnVal;
    if ((returnVal = addGyroscope           (pmmImuStructPtr->gyroscopeArray)))     return returnVal;
    if ((returnVal = addTemperatureMpu      (&pmmImuStructPtr->temperatureMpu)))    return returnVal;

    if ((returnVal = addMagnetometer        (pmmImuStructPtr->magnetometerArray)))  return returnVal;

    if ((returnVal = addBarometer           (&pmmImuStructPtr->pressure)))          return returnVal;
    if ((returnVal = addRawAltitudeBarometer(&pmmImuStructPtr->altitude)))          return returnVal;
    if ((returnVal = addAltitudeBarometer   (&pmmImuStructPtr->filteredAltitude)))  return returnVal;
    if ((returnVal = addTemperatureBmp      (&pmmImuStructPtr->temperatureBmp)))    return returnVal;
    
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
