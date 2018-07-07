#include <pmmPackageLog.h>
#include <pmmConsts.h>

uint8_t PmmPackageLog::variableTypeToVariableSize(uint8_t variableType)
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

void PmmPackageLog::includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress)
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

void PmmPackageLog::addPackageBasicInfo(uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{
    const PROGMEM char* packageIdString = "packageId";
    const PROGMEM char* packageTimeString = "packageTime(ms)";
    includeVariableInPackage(packageIdString, PMM_TELEMETRY_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(packageTimeString, PMM_TELEMETRY_TYPE_UINT32, packageTimeMsPtr);
}

void PmmPackageLog::addMagnetometer(float* magnetometerArray[3])
{
    const PROGMEM char* magnetometerXString = "magnetometerX(uT)";
    const PROGMEM char* magnetometerYString = "magnetometerX(uT)";
    const PROGMEM char* magnetometerZString = "magnetometerX(uT)";
    includeVariableInPackage(magnetometerXString, PMM_TELEMETRY_TYPE_FLOAT, &magnetometerArray[0]);
    includeVariableInPackage(magnetometerYString, PMM_TELEMETRY_TYPE_FLOAT, &magnetometerArray[1]);
    includeVariableInPackage(magnetometerZString, PMM_TELEMETRY_TYPE_FLOAT, &magnetometerArray[2]);
}

void PmmPackageLog::addGyroscope(float* gyroscopeArray[3])
{
    const PROGMEM char* gyroscopeXString = "gyroscopeX()";
    const PROGMEM char* gyroscopeYString = "gyroscopeY()";
    const PROGMEM char* gyroscopeZString = "gyroscopeZ()";
    includeVariableInPackage(gyroscopeXString, PMM_TELEMETRY_TYPE_FLOAT, &gyroscopeArray[0]);
    includeVariableInPackage(gyroscopeYString, PMM_TELEMETRY_TYPE_FLOAT, &gyroscopeArray[1]);
    includeVariableInPackage(gyroscopeZString, PMM_TELEMETRY_TYPE_FLOAT, &gyroscopeArray[2]);
}

void PmmPackageLog::addAccelerometer(float* accelerometerArray[3])
{
    const PROGMEM char* accelerometerXString = "accelerometerX(m/s^2)";
    const PROGMEM char* accelerometerYString = "accelerometerY(m/s^2)";
    const PROGMEM char* accelerometerZString = "accelerometerZ(m/s^2)";
    includeVariableInPackage(accelerometerXString, PMM_TELEMETRY_TYPE_FLOAT, &accelerometerArray[0]);
    includeVariableInPackage(accelerometerYString, PMM_TELEMETRY_TYPE_FLOAT, &accelerometerArray[1]);
    includeVariableInPackage(accelerometerZString, PMM_TELEMETRY_TYPE_FLOAT, &accelerometerArray[2]);
}

void PmmPackageLog::addBarometer(float* barometer)
{
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";

    includeVariableInPackage(barometerPressureString, PMM_TELEMETRY_TYPE_FLOAT, &barometerArray);

}

void PmmPackageLog::addAltitudeBarometer(float* altitudeBarometer)
{
    const PROGMEM char* barometerAltitudeString = "barometerAltitude(m)";
    includeVariableInPackage(barometerAltitudeString, PMM_TELEMETRY_TYPE_FLOAT, &barometerArray);
}

void PmmPackageLog::addThermometer(float* thermometerPtr)
{
    const PROGMEM char* thermometerString = "temperature(C)";
    includeVariableInPackage(thermometerString, PMM_TELEMETRY_TYPE_FLOAT, thermometerPtr);
}

void PmmPackageLog::addImu(pmmImuStructType *pmmImuStructPtr)
{
    addMagnetometer(&pmmImuStructPtr.magnetometer);
    addGyroscope(&pmmImuStructPtr.gyroscope);
    addAccelerometer(&pmmImuStructPtr.accelerometer);
    addBarometer(&pmmImuStructPtr.barometer);
    addAltitudeBarometer(&pmmImuStructPtr.altitude);
    addThermometer(&pmmImuStructPtr.thermometer);
}

void PmmPackageLog::addGps(pmmGpsStructType pmmGpsStruct)
{
    #ifdef GPS_FIX_LOCATION
        const PROGMEM char* gpsLatitudeString = "gpsLatitude";
        const PROGMEM char* gpsLongitudeString = "gpsLongitude";
        includeVariableInPackage(gpsLatitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.latitude));
        includeVariableInPackage(gpsLongitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.longitude));
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char* gpsAltitudeString = "gpsAltitude(m)";
        includeVariableInPackage(gpsAltitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.altitude));
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char* gpsSatellitesString = "gpsSatellites";
        includeVariableInPackage(gpsSatellitesString, PMM_TELEMETRY_TYPE_UINT8, &(pmmGpsStruct.satellites));
    #endif

    #ifdef GPS_FIX_SPEED
        const PROGMEM char* gpsHorizontalSpeedString = "gpsHorSpeed(m/s)";
        const PROGMEM char* gpsNorthSpeedString = "gpsNorthSpeed(m/s)";
        const PROGMEM char* gpsEastSpeedString = "gpsEastSpeed(m/s)";
        const PROGMEM char* gpsHeadingDegreeString = "gpsHeadingDegree";
        includeVariableInPackage(gpsHorizontalSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.horizontalSpeed));
        includeVariableInPackage(gpsNorthSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.northSpeed));
        includeVariableInPackage(gpsEastSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.eastSpeed));
        includeVariableInPackage(gpsHeadingDegreeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct.headingDegree));

        #ifdef GPS_FIX_ALTITUDE
            const PROGMEM char* gpsUpSpeedString = "gpsSpeedUp(m/s)";
            includeVariableInPackage(gpsUpSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &pmmGpsStruct.upSpeed);
        #endif
    #endif
}



unsigned PmmPackageLog::returnNumberOfVariables()
{
    return mActualNumberVariables;
}

void PmmPackageLog::addCustomVariable(const char* variableName, uint8_t variableType, void* variableAddress)
{
    includeVariableInPackage(variableName, variableType, variableAddress);
}

// Note for the 2 functions below:
// There are faster ways to print the debugs, but since it isn't something that is going to be used frequently,
// I (HB :) ) will spend my precious time on other stuffs)
#if PMM_SERIAL_DEBUG
void PmmPackageLog::debugPrintLogHeader()
{
    unsigned variableIndex;
    char buffer[512]; // No static needed, as it is called only once.
    for (variableIndex = 0; variableIndex < mActualNumberVariables; variableIndex ++)
    {
        strncat(buffer, mVariableName[variableIndex], 512);
        Serial.print(buffer);
    }
}
void PmmPackageLog::debugPrintLogContent()
{
    unsigned variableIndex;
    static char buffer[512]; // Static for optimization
    for (variableIndex = 0; variableIndex < mActualNumberVariables; variableIndex ++)
    {
        switch(mVariableType[variableIndex])
        {
            case PMM_TELEMETRY_TYPE_FLOAT: // first as it is more common
                snprintf(buffer, 512, "%s%f,", buffer, *(float*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT32:
                snprintf(buffer, 512, "%s%lu,", buffer, *(uint32_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT32:
                snprintf(buffer, 512, "%s%li,", buffer, *(int32_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT8:
                snprintf(buffer, 512, "%s%u,", buffer, *(uint8_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT8:
                snprintf(buffer, 512, "%s%i,", buffer, *(int8_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT16:
                snprintf(buffer, 512, "%s%u,", buffer, *(uint16_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT16:
                snprintf(buffer, 512, "%s%i,", buffer, *(int16_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT64:
                snprintf(buffer, 512, "%s%llu,", buffer, *(uint64_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT64:
                snprintf(buffer, 512, "%s%lli,", buffer, *(int64_t*) (mVariableAddress[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_DOUBLE:
                snprintf(buffer, 512, "%s%f,", buffer, *(double*) (mVariableAddress[variableIndex]));
                break;
            default:    // If none above,
                snprintf(buffer, 512, "%s%s,", buffer, ">TYPE ERROR HERE!<");
                break;
        } // switch end
        Serial.println(buffer);
    } // for loop end
} // end of function debugPrintLogContent()


#endif
