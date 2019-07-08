#include "pmmGps/gps.h"                  // For GPS struct
#include "pmmImu/imu.h"                  // For IMU struct
#include "pmmModules/dataLog/dataLog.h"


PmmModuleDataLogGroupCore::PmmModuleDataLogGroupCore()
{
    reset();
}

int PmmModuleDataLogGroupCore::init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, PmmSdSafeLog* pmmSdSafeLog, uint8_t systemSession, uint8_t dataLogGroupId)
{
    mPmmTelemetryPtr   = pmmTelemetry;
    mPmmSdPtr          = pmmSd;
    mPmmSdSafeLogPtr   = pmmSdSafeLog;

    mDataLogGroupId = dataLogGroupId;
    mSystemSession  = systemSession;

    return 0;
}

int PmmModuleDataLogGroupCore::reset()
{
    mTransmissionCounter = 0;
    mIsGroupLocked       = 0;
    mDataLogGroupId      = 0;
    mNumberVariables     = 0;
    mGroupLength         = 0;

    mDataLogInfoPackets = 0;
    return 0;
}

uint8_t variableTypeToVariableSize(uint8_t variableType)
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
            advPrintf("Invalid variable type to size!\n");
            return 1;
    }
}

int PmmModuleDataLogGroupCore::includeVariable(const char *variableName, uint8_t variableType, void *variableAddress)
{
    if (!variableName)
        return 1;

    if (!variableAddress)
        return 2;

    if (mIsGroupLocked)
    {
        advPrintf("Failed to add the variable \"%s\". DataLog is already locked.\n", variableName)
        return 3;
    }

    uint8_t varSize = variableTypeToVariableSize(variableType);

    if (mNumberVariables >= MODULE_DATA_LOG_MAX_VARIABLES)
    {
        advPrintf("Failed to add the variable \"%s\". Exceeds the maximum number of variables in the DataLog.", variableName)
        return 4;
    }

    if ((mGroupLength + varSize) >= PORT_DATA_LOG_MAX_PAYLOAD_LENGTH)
    {
        advPrintf("Failed to add the variable \"%s\". Exceeds the maximum content byte size (tried to be %u, max is %u).", variableName, mGroupLength + varSize, PORT_DATA_LOG_MAX_PAYLOAD_LENGTH)
        return 5;
    }

    mVariableNameArray[mNumberVariables] = (char*) variableName; // Typecast from (const char*) to (char*)
    mVariableTypeArray[mNumberVariables] = variableType;
    mVariableSizeArray[mNumberVariables] = varSize;
    mVariableAdrsArray[mNumberVariables] = (uint8_t*) variableAddress;
    mNumberVariables++;
    mGroupLength    += varSize;

    return 0;
}



int PmmModuleDataLogGroupCore::includeArray(const char **variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize)
{
    if (!variableName)
        return 1;
    if (!arrayAddress)
        return 2;

    uint8_t counter;
    for (counter = 0; counter < arraySize; counter++)
        includeVariable(*variableName++, arrayType, (uint8_t*) arrayAddress + (variableTypeToVariableSize(arrayType) * counter));

    return 0;
}



int PmmModuleDataLogGroupCore::addTransmissionCounter()
{
    return includeVariable(PMM_DATA_LOG_TRANSMISSION_COUNTER_STRING, MODULE_DATA_LOG_TYPE_UINT32, &mTransmissionCounter);
}

int PmmModuleDataLogGroupCore::addMainLoopCounter    (uint32_t* mainLoopCounterPtr)
{
    return includeVariable(PMM_DATA_LOG_MAIN_LOOP_COUNTER_STRING,   MODULE_DATA_LOG_TYPE_UINT32, mainLoopCounterPtr);
}

int PmmModuleDataLogGroupCore::addTimeMillis         (uint32_t* timeMillisPtr)
{
    return includeVariable(PMM_DATA_LOG_SYSTEM_TIME_MILLIS_STRING,   MODULE_DATA_LOG_TYPE_UINT32, timeMillisPtr);
}

int PmmModuleDataLogGroupCore::addBasicInfo          (uint32_t* mainLoopCounter, uint32_t* timeMillis) // Adds the three above.
{
    int returnValue;

    if ((returnValue = addTransmissionCounter()))               return returnValue;
    if ((returnValue = addMainLoopCounter(mainLoopCounter)))    return returnValue;
    if ((returnValue = addTimeMillis(timeMillis)))              return returnValue;

    return 0;
}



int PmmModuleDataLogGroupCore::addAccelerometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"accelerometerX(g)", "accelerometerY(g)", "accelerometerZ(g)"};
    return includeArray(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}

int PmmModuleDataLogGroupCore::addGyroscope(void* array)
{
    const PROGMEM char* arrayString[3] = {"gyroscopeX(degree/s)", "gyroscopeY(degree/s)", "gyroscopeZ(degree/s)"};
    return includeArray(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}

int PmmModuleDataLogGroupCore::addTemperatureMpu(void* temperatureMpu)
{
    const PROGMEM char* mpuTemperatureString = "temperatureMpu(C)";
    return includeVariable(mpuTemperatureString, MODULE_DATA_LOG_TYPE_FLOAT, temperatureMpu);
}

int PmmModuleDataLogGroupCore::addMagnetometer(void* array)
{
    const PROGMEM char* arrayString[3] = {"magnetometerX(uT)", "magnetometerY(uT)", "magnetometerZ(uT)"};
    return includeArray(arrayString, MODULE_DATA_LOG_TYPE_FLOAT, array, 3);
}

int PmmModuleDataLogGroupCore::addBarometer(void* barometer)
{
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";
    return includeVariable(barometerPressureString, MODULE_DATA_LOG_TYPE_FLOAT, barometer);
}

// Without filtering
int PmmModuleDataLogGroupCore::addRawAltitudeBarometer(void* rawAltitudePressure)
{
    return includeVariable(PMM_DATA_LOG_RAW_ALTITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, rawAltitudePressure);
}

int PmmModuleDataLogGroupCore::addAltitudeBarometer(void* altitude)
{
    return includeVariable(PMM_DATA_LOG_ALTITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, altitude);
}

int PmmModuleDataLogGroupCore::addTemperatureBmp(void* barometerTempPtr)
{
    const PROGMEM char* barometerTempString = "temperatureBmp(C)";
    return includeVariable(barometerTempString, MODULE_DATA_LOG_TYPE_FLOAT, barometerTempPtr);
}



int PmmModuleDataLogGroupCore::addImu(pmmImuStructType *pmmImuStructPtr)
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



int PmmModuleDataLogGroupCore::addGps(pmmGpsStructType* pmmGpsStruct)
{
    int returnVal;

    #ifdef GPS_FIX_LOCATION
        if ((returnVal = includeVariable(PMM_DATA_LOG_GPS_LATITUDE_STRING,  MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->latitude ))) return returnVal;
        if ((returnVal = includeVariable(PMM_DATA_LOG_GPS_LONGITUDE_STRING, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->longitude))) return returnVal;
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char* gpsAltitudeString        = "gpsAltitude(m)";
        if ((returnVal = includeVariable(gpsAltitudeString,      MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->altitude)))      return returnVal;
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char* gpsSatellitesString      = "gpsSatellites";
        if ((returnVal = includeVariable(gpsSatellitesString,    MODULE_DATA_LOG_TYPE_UINT8, &pmmGpsStruct->satellites)))    return returnVal;
    #endif

    #ifdef GPS_FIX_HEADING
        const PROGMEM char* gpsHeadingDegreeString   = "gpsHeadingDegree";
        if ((returnVal = includeVariable(gpsHeadingDegreeString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->headingDegree))) return returnVal;
    #endif

    #ifdef GPS_FIX_SPEED
        const PROGMEM char* gpsUpSpeedString         = "gpsSpeedUp(m/s)";
        const PROGMEM char* gpsHorizontalSpeedString = "gpsHorizontalSpeed(m/s)";
        if ((returnVal = includeVariable(gpsUpSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->upSpeed))) return returnVal;
        if ((returnVal = includeVariable(gpsHorizontalSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->horizontalSpeed))) return returnVal;

        #ifdef GPS_FIX_HEADING
            const PROGMEM char* gpsNorthSpeedString  = "gpsNorthSpeed(m/s)";
            const PROGMEM char* gpsEastSpeedString   = "gpsEastSpeed(m/s)";
            if ((returnVal = includeVariable(gpsNorthSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->northSpeed))) return returnVal;
            if ((returnVal = includeVariable(gpsEastSpeedString, MODULE_DATA_LOG_TYPE_FLOAT, &pmmGpsStruct->eastSpeed))) return returnVal;
        #endif
    #endif

    return 0;
}



int PmmModuleDataLogGroupCore::addCustomVariable(const char* variableName, uint8_t variableType, void* variableAddress)
{
    return includeVariable(variableName, variableType, variableAddress);
}


/* Getters! -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
uint8_t PmmModuleDataLogGroupCore::getNumberOfVariables()       { return mNumberVariables; }
int     PmmModuleDataLogGroupCore::getIsGroupLocked()           { return mIsGroupLocked;   }


const char** PmmModuleDataLogGroupCore::getVariableNameArray()  { return (const char**) mVariableNameArray  ;}
uint8_t*     PmmModuleDataLogGroupCore::getVariableTypeArray()  { return mVariableTypeArray                 ;}
uint8_t*     PmmModuleDataLogGroupCore::getVariableSizeArray()  { return mVariableSizeArray                 ;}
uint8_t**    PmmModuleDataLogGroupCore::getVariableAdrsArray()  { return mVariableAdrsArray                 ;}


uint8_t      PmmModuleDataLogGroupCore::getDataLogInfoPackets() { return mDataLogInfoPackets                ;}