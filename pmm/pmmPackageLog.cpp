#include <pmmPackageLog.h>
#include <pmmConsts.h>

#define typeToSize(x) _Generic((x), \
        uint8_t:    1, \
        int8_t:     1, \
        uint16_t:   2, \
        int16_t:    2, \
        uint32_t:   4, \
        int32_t:    4, \
        float:      4, \
        uint64_t:   8, \
        int64_t:    8, \
        double:     8, \
        default:    0)

#define typeToIdentifier(x) _Generic((x), \
        uint8_t:    0, \
        int8_t:     1, \
        uint16_t:   2, \
        int16_t:    3, \
        uint32_t:   4, \
        int32_t:    5, \
        float:      6, \
        uint64_t:   8, \
        int64_t:    9, \
        double:     10, \
        default:    0)

PmmPackageLog::PmmPackageLog()
{
    mPackageSizeInBytes = 0;
    mActualNumberVariables = 0;
}
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
    uint8_t varSize = variableTypeToVariableSize(variableType);
    if (mActualNumberVariables >= PMM_TELEMETRY_LOG_NUMBER_VARIABLES)
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("PmmPackage #1: Failed to add the variable \"");
            Serial.print(variableName);
            Serial.print("\". Exceeds the maximum number of variables in the Package Log.\n");
        #endif
        return;
    }
    if ((mPackageSizeInBytes + varSize) <= PMM_TELEMETRY_MAX_PAYLOAD_LENGTH)
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("PmmPackage #2: Failed to add the variable \"");
            Serial.print(variableName);
            Serial.print("\". Exceeds the maximum payload length (tried to be ");
            Serial.print((mPackageSizeInBytes + varSize));
            Serial.print(", maximum is ");
            Serial.print(PMM_TELEMETRY_MAX_PAYLOAD_LENGTH);
            Serial.print(".");
        #endif
        return;
    }

    mVariableName[mActualNumberVariables] = variableName;
    mVariableType[mActualNumberVariables] = variableType;
    mVariableSize[mActualNumberVariables] = varSize;
    mVariableAddress[mActualNumberVariables] = (uint8_t*) variableAddress;
    mActualNumberVariables ++;
    mPackageSizeInBytes += varSize;

}

void PmmPackageLog::addPackageBasicInfo(uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{
    const PROGMEM char* packageIdString = "packageId";
    const PROGMEM char* packageTimeString = "packageTime(ms)";
    includeVariableInPackage(packageIdString, PMM_TELEMETRY_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(packageTimeString, PMM_TELEMETRY_TYPE_UINT32, packageTimeMsPtr);
}


void PmmPackageLog::addMagnetometer(PMM_PACKAGE_LOG_MAGNETOMETER_TYPE magnetometerArray[3])
{
    const PROGMEM char* magnetometerXString = "magnetometerX(uT)";
    const PROGMEM char* magnetometerYString = "magnetometerY(uT)";
    const PROGMEM char* magnetometerZString = "magnetometerZ(uT)";
    includeVariableInPackage(magnetometerXString, typeToIdentifier(magnetometerArray[0]), &magnetometerArray[0]);
    includeVariableInPackage(magnetometerYString, typeToIdentifier(magnetometerArray[1]), &magnetometerArray[1]);
    includeVariableInPackage(magnetometerZString, typeToIdentifier(magnetometerArray[2]), &magnetometerArray[2]);
}

void PmmPackageLog::addGyroscope(PMM_PACKAGE_LOG_GYROSCOPE_TYPE gyroscopeArray[3])
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even faster (Change ONE line instead of THREE!) type definition!
    const PROGMEM char* gyroscopeXString = "gyroscopeX()";
    const PROGMEM char* gyroscopeYString = "gyroscopeY()";
    const PROGMEM char* gyroscopeZString = "gyroscopeZ()";
    includeVariableInPackage(gyroscopeXString, type, &gyroscopeArray[0]);
    includeVariableInPackage(gyroscopeYString, type, &gyroscopeArray[1]);
    includeVariableInPackage(gyroscopeZString, type, &gyroscopeArray[2]);
}

void PmmPackageLog::addAccelerometer(PMM_PACKAGE_LOG_ACCELEROMETER_TYPE accelerometerArray[3])
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even faster (Change ONE line instead of THREE!) type definition!
    const PROGMEM char* accelerometerXString = "accelerometerX(m/s^2)";
    const PROGMEM char* accelerometerYString = "accelerometerY(m/s^2)";
    const PROGMEM char* accelerometerZString = "accelerometerZ(m/s^2)";
    includeVariableInPackage(accelerometerXString, type, &accelerometerArray[0]);
    includeVariableInPackage(accelerometerYString, type, &accelerometerArray[1]);
    includeVariableInPackage(accelerometerZString, type, &accelerometerArray[2]);
}

void PmmPackageLog::addBarometer(PMM_PACKAGE_LOG_BAROMETER_TYPE* barometer)
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even CLEAR (change a variable instead of changing the function argument!) type definition!
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";
    includeVariableInPackage(barometerPressureString, type, &barometerArray);

}

void PmmPackageLog::addAltitudeBarometer(PMM_PACKAGE_LOG_ALTITUDE_BAROMETER_TYPE* altitudeBarometer)
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even CLEAR (change a variable instead of changing the function argument!) type definition!
    const PROGMEM char* barometerAltitudeString = "barometerAltitude(m)";
    includeVariableInPackage(barometerAltitudeString, PMM_TELEMETRY_TYPE_FLOAT, &barometerArray);
}

void PmmPackageLog::addThermometer(PMM_PACKAGE_THERMOMETER_TYPE* thermometerPtr)
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even CLEAR (change a variable instead of changing the function argument!) type definition!
    const PROGMEM char* thermometerString = "temperature(C)";
    includeVariableInPackage(thermometerString, type, thermometerPtr);
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

uint8_t PmmPackageLog::returnPackageSizeInBytes()
{
    return mPackageSizeInBytes;
}




#if PMM_DEBUG_SERIAL
// Note for the 2 functions below:
// There are faster ways to print the debugs, but since it isn't something that is going to be used frequently,
// I (HB :) ) will spend my precious time on other stuffs)

void PmmPackageLog::debugPrintLogHeader()
{
    unsigned variableIndex;
    char buffer[512]; // No static needed, as it is called only once.
    for (variableIndex = 0; variableIndex < mActualNumberVariables; variableIndex ++)
    {
        strncat(buffer, mVariableName[variableIndex], 512);
        Serial.println(buffer);
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
