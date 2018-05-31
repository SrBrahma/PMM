#include <pmmPackage.h>

uint8_t PmmLogPackage::variableTypeToVariableSize(uint8_t variableType)
{
    switch (variableType)
    {
        case PMM_TELEMETRY_TYPE_UINT8:
            return 1;
        case PMM_TELEMETRY_TYPE_INT8:
            return 1;
        case PMM_TELEMETRY_TYPE_UINT16:
            return 2;
        case PMM_TELEMETRY_TYPE_INT16:
            return 2;
        case PMM_TELEMETRY_TYPE_UINT32:
            return 4;
        case PMM_TELEMETRY_TYPE_INT32:
            return 4;
        case PMM_TELEMETRY_TYPE_FLOAT:
            return 4;
        case PMM_TELEMETRY_TYPE_UINT64:
            return 8;
        case PMM_TELEMETRY_TYPE_INT64:
            return 8;
        case PMM_TELEMETRY_TYPE_DOUBLE:
            return 8;
        default:    // Maybe will avoid internal crashes?
            return 1;
    }
}

void PmmLogPackage::includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress)
{
    if (mActualNumberVariables < PMM_TELEMETRY_LOG_NUMBER_VARIABLES)
    {
        mVariableName[mActualNumberVariables] = variableName;
        mVariableType[mActualNumberVariables] = variableType;
        mVariableSize[mActualNumberVariables] = variableTypeToVariableSize(variableType);
        mVariableAddress[mActualNumberVariables] = (uint8_t *) variableAddress;
        mActualNumberVariables ++;
    }

}

void PmmLogPackage::addPackageInfo(uint32_t * packageIdPtr, uint32_t * packageTimeMsPtr)
{
    const PROGMEM char * packageIdString = "packageId";
    const PROGMEM char * packageTimeString = "packageTime(ms)";
    includeVariableInPackage(packageIdString, PMM_TELEMETRY_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(packageTimeString, PMM_TELEMETRY_TYPE_UINT32, packageTimeMsPtr);
}

void PmmLogPackage::addMagnetometer(float magnetometerArray[3])
{
    const PROGMEM char * magnetometerXString = "magnetometerX(uT)";
    const PROGMEM char * magnetometerYString = "magnetometerX(uT)";
    const PROGMEM char * magnetometerZString = "magnetometerX(uT)";

    includeVariableInPackage(magnetometerXString, PMM_TELEMETRY_TYPE_FLOAT, &magnetometerArray[0]);
    includeVariableInPackage(magnetometerYString, PMM_TELEMETRY_TYPE_FLOAT, &magnetometerArray[1]);
    includeVariableInPackage(magnetometerZString, PMM_TELEMETRY_TYPE_FLOAT, &magnetometerArray[2]);
}

void PmmLogPackage::addGyroscope(float gyroscopeArray[3])
{
    const PROGMEM char * gyroscopeXString = "gyroscopeX()";
    const PROGMEM char * gyroscopeYString = "gyroscopeY()";
    const PROGMEM char * gyroscopeZString = "gyroscopeZ()";

    includeVariableInPackage(gyroscopeXString, PMM_TELEMETRY_TYPE_FLOAT, &gyroscopeArray[0]);
    includeVariableInPackage(gyroscopeYString, PMM_TELEMETRY_TYPE_FLOAT, &gyroscopeArray[1]);
    includeVariableInPackage(gyroscopeZString, PMM_TELEMETRY_TYPE_FLOAT, &gyroscopeArray[2]);
}

void PmmLogPackage::addAccelerometer(float accelerometerArray[3])
{
    const PROGMEM char * accelerometerXString = "accelerometerX(m/s^2)";
    const PROGMEM char * accelerometerYString = "accelerometerY(m/s^2)";
    const PROGMEM char * accelerometerZString = "accelerometerZ(m/s^2)";
    includeVariableInPackage(accelerometerXString, PMM_TELEMETRY_TYPE_FLOAT, &accelerometerArray[0]);
    includeVariableInPackage(accelerometerYString, PMM_TELEMETRY_TYPE_FLOAT, &accelerometerArray[1]);
    includeVariableInPackage(accelerometerZString, PMM_TELEMETRY_TYPE_FLOAT, &accelerometerArray[2]);
}

void PmmLogPackage::addBarometer(float barometerArray[2])
{
    const PROGMEM char * barometerPressureString = "barometerPressure(Pa)";
    const PROGMEM char * barometerAltitudeString = "barometerAltitude(m)";
    includeVariableInPackage(barometerPressureString, PMM_TELEMETRY_TYPE_FLOAT, &barometerArray[0]);
    includeVariableInPackage(barometerAltitudeString, PMM_TELEMETRY_TYPE_FLOAT, &barometerArray[1]);
}



void PmmLogPackage::addThermometer(float *thermometerPtr)
{
    const PROGMEM char * thermometerString = "temperature(C)";
    includeVariableInPackage(thermometerString, PMM_TELEMETRY_TYPE_FLOAT, thermometerPtr);
}



void PmmLogPackage::addGps(gpsStructType *gpsStruct)
{
    #ifdef GPS_FIX_LOCATION
        const PROGMEM char * gpsLatitudeString = "gpsLatitude";
        const PROGMEM char * gpsLongitudeString = "gpsLongitude";
        includeVariableInPackage(gpsLatitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->latitude));
        includeVariableInPackage(gpsLongitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->longitude));
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char * gpsAltitudeString = "gpsAltitude(m)";
        includeVariableInPackage(gpsAltitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->altitude));
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char * gpsSatellitesString = "gpsSatellites";
        includeVariableInPackage(gpsSatellitesString, PMM_TELEMETRY_TYPE_UINT8, &(gpsStruct->satellites));
    #endif

    #ifdef GPS_FIX_SPEED
        const PROGMEM char * gpsHorizontalSpeedString = "gpsHorSpeed(m/s)";
        const PROGMEM char * gpsNorthSpeedString = "gpsNorthSpeed(m/s)";
        const PROGMEM char * gpsEastSpeedString = "gpsEastSpeed(m/s)";
        const PROGMEM char * gpsHeadingDegreeString = "gpsHeadingDegree";
        includeVariableInPackage(gpsHorizontalSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->horizontalSpeed));
        includeVariableInPackage(gpsNorthSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->northSpeed));
        includeVariableInPackage(gpsEastSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->eastSpeed));
        includeVariableInPackage(gpsHeadingDegreeString, PMM_TELEMETRY_TYPE_FLOAT, &(gpsStruct->headingDegree));

        #ifdef GPS_FIX_ALTITUDE
            const PROGMEM char * gpsUpSpeedString = "gpsSpeedUp(m/s)";
            includeVariableInPackage(gpsUpSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &gpsStruct->upSpeed);
        #endif
    #endif
}

unsigned PmmLogPackage::returnNumberOfVariables()
{
    return mActualNumberVariables;
}

void PmmLogPackage::addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress)
{
    includeVariableInPackage(variableName, variableType, variableAddress);
}
