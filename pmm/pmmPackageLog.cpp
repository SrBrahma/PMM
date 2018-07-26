/* pmmPackageLog.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#define PMM_PACKAGE_LOG_DATA_INDEX 2
// 0 is MLOG
// 1 is MLIN String CRC
// 2 is data (PMM_PACKAGE_LOG_DATA_INDEX)
#include <crc16.h>
#include <pmmPackageLog.h>
#include <pmmConsts.h>



PmmPackageLog::PmmPackageLog()
{
    mPackageSizeInBytes = 0;
    mActualNumberVariables = 0;

    const PROGMEM char* packageLogHeader = "packageLogHeader"; // It isn't actually used. But will leave it for the future. (CMON 11 BYTES AT PROGMEM IS NOTHING)
    addCustomVariable(packageLogHeader, PMM_TELEMETRY_TYPE_UINT32, (void*)PMM_TELEMETRY_HEADER_LOG); // MLOG, the header. (void*) is to convert const* void to void*.

    const PROGMEM char* mlinStrincCrc = "mlinStrCrc"; // Same as the above funny commentary. Maybe you can't find it, it isn't funny at all.
    addCustomVariable(mlinStrincCrc, PMM_TELEMETRY_TYPE_UINT16, &mMlinStringCrc); // MLIN String CRC
}



void PmmPackageLog::updateMlinStringCrc()
{
    char buffer[512] = {0}; // No static needed, as it is called only once.
    unsigned variableIndex;
    for (variableIndex = PMM_PACKAGE_LOG_DATA_INDEX; variableIndex < mActualNumberVariables; variableIndex ++)
        strncat(buffer, mVariableNameArray[variableIndex], 512);

    //mMlinStringCrc = crc16(buffer, strlen(buffer));
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
            PMM_DEBUG_PRINT("PmmPackage #1: Invalid variable type to size!");
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
    if ((mPackageSizeInBytes + varSize) >= PMM_TELEMETRY_MAX_PAYLOAD_LENGTH)
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("PmmPackage #2: Failed to add the variable \"");
            Serial.print(variableName);
            Serial.print("\". Exceeds the maximum payload length (tried to be ");
            Serial.print((mPackageSizeInBytes + varSize));
            Serial.print(", maximum is ");
            Serial.print(PMM_TELEMETRY_MAX_PAYLOAD_LENGTH);
            Serial.print(".\n");
        #endif
        return;
    }

    mVariableNameArray[mActualNumberVariables] = variableName;
    mVariableTypeArray[mActualNumberVariables] = variableType;
    mVariableSizeArray[mActualNumberVariables] = varSize;
    mVariableAddressArray[mActualNumberVariables] = (uint8_t*) variableAddress;
    mActualNumberVariables ++;
    mPackageSizeInBytes += varSize;

    if (mActualNumberVariables > PMM_PACKAGE_LOG_DATA_INDEX) // yeah it's right. It isn't actually necessary, just skip a few useless function calls.
        updateMlinStringCrc(); // Updates the Mlin string CRC.
}

void PmmPackageLog::addPackageBasicInfo(uint32_t* packageIdPtr, uint32_t* packageTimeMsPtr)
{
    const PROGMEM char* packageIdString = "packageId";
    const PROGMEM char* packageTimeString = "packageTime(ms)";
    includeVariableInPackage(packageIdString, PMM_TELEMETRY_TYPE_UINT32, packageIdPtr);
    includeVariableInPackage(packageTimeString, PMM_TELEMETRY_TYPE_UINT32, packageTimeMsPtr);
}


void PmmPackageLog::addMagnetometer(float magnetometerArray[3])
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even faster (Change ONE line instead of THREE!) type definition!
    const PROGMEM char* magnetometerXString = "magnetometerX(uT)";
    const PROGMEM char* magnetometerYString = "magnetometerY(uT)";
    const PROGMEM char* magnetometerZString = "magnetometerZ(uT)";
    includeVariableInPackage(magnetometerXString, type, &magnetometerArray[0]);
    includeVariableInPackage(magnetometerYString, type, &magnetometerArray[1]);
    includeVariableInPackage(magnetometerZString, type, &magnetometerArray[2]);
}

void PmmPackageLog::addGyroscope(float gyroscopeArray[3])
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even faster (Change ONE line instead of THREE!) type definition!
    const PROGMEM char* gyroscopeXString = "gyroscopeX(degree/s)";
    const PROGMEM char* gyroscopeYString = "gyroscopeY(degree/s)";
    const PROGMEM char* gyroscopeZString = "gyroscopeZ(degree/s)";
    includeVariableInPackage(gyroscopeXString, type, &gyroscopeArray[0]);
    includeVariableInPackage(gyroscopeYString, type, &gyroscopeArray[1]);
    includeVariableInPackage(gyroscopeZString, type, &gyroscopeArray[2]);
}

void PmmPackageLog::addAccelerometer(float accelerometerArray[3])
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even faster (Change ONE line instead of THREE!) type definition!
    const PROGMEM char* accelerometerXString = "accelerometerX(g)";
    const PROGMEM char* accelerometerYString = "accelerometerY(g)";
    const PROGMEM char* accelerometerZString = "accelerometerZ(g)";
    includeVariableInPackage(accelerometerXString, type, &accelerometerArray[0]);
    includeVariableInPackage(accelerometerYString, type, &accelerometerArray[1]);
    includeVariableInPackage(accelerometerZString, type, &accelerometerArray[2]);
}

void PmmPackageLog::addBarometer(float* barometer)
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even CLEAR (change a variable instead of changing the function argument!) type definition!
    const PROGMEM char* barometerPressureString = "barometerPressure(hPa)";
    includeVariableInPackage(barometerPressureString, type, &barometer);

}

void PmmPackageLog::addAltitudeBarometer(float* altitudePressure)
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even CLEAR (change a variable instead of changing the function argument!) type definition!
    const PROGMEM char* barometerAltitudeString = "barometerAltitude(m)";
    includeVariableInPackage(barometerAltitudeString, type, &altitudePressure);
}

void PmmPackageLog::addThermometer(float* thermometerPtr)
{
    uint8_t type = PMM_TELEMETRY_TYPE_FLOAT; // For an even CLEAR (change a variable instead of changing the function argument!) type definition!
    const PROGMEM char* thermometerString = "temperature(C)";
    includeVariableInPackage(thermometerString, type, thermometerPtr);
}

void PmmPackageLog::addImu(pmmImuStructType *pmmImuStructPtr)
{
    addAccelerometer(pmmImuStructPtr->accelerometerArray);
    addGyroscope(pmmImuStructPtr->gyroscopeArray);
    addMagnetometer(pmmImuStructPtr->magnetometerArray);

    addBarometer(&pmmImuStructPtr->pressure);
    addAltitudeBarometer(&pmmImuStructPtr->altitudePressure);
    addThermometer(&pmmImuStructPtr->temperature);
}

void PmmPackageLog::addGps(pmmGpsStructType* pmmGpsStruct)
{
    #ifdef GPS_FIX_LOCATION
        const PROGMEM char* gpsLatitudeString = "gpsLatitude";
        const PROGMEM char* gpsLongitudeString = "gpsLongitude";
        includeVariableInPackage(gpsLatitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->latitude));
        includeVariableInPackage(gpsLongitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->longitude));
    #endif

    #ifdef GPS_FIX_ALTITUDE
        const PROGMEM char* gpsAltitudeString = "gpsAltitude(m)";
        includeVariableInPackage(gpsAltitudeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->altitude));
    #endif

    #ifdef GPS_FIX_SATELLITES
        const PROGMEM char* gpsSatellitesString = "gpsSatellites";
        includeVariableInPackage(gpsSatellitesString, PMM_TELEMETRY_TYPE_UINT8, &(pmmGpsStruct->satellites));
    #endif
    /*
    #ifdef GPS_FIX_SPEED
        const PROGMEM char* gpsHorizontalSpeedString = "gpsHorSpeed(m/s)";
        const PROGMEM char* gpsNorthSpeedString = "gpsNorthSpeed(m/s)";
        const PROGMEM char* gpsEastSpeedString = "gpsEastSpeed(m/s)";
        const PROGMEM char* gpsHeadingDegreeString = "gpsHeadingDegree";
        includeVariableInPackage(gpsHorizontalSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->horizontalSpeed));
        includeVariableInPackage(gpsNorthSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->northSpeed));
        includeVariableInPackage(gpsEastSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->eastSpeed));
        includeVariableInPackage(gpsHeadingDegreeString, PMM_TELEMETRY_TYPE_FLOAT, &(pmmGpsStruct->headingDegree));

        #ifdef GPS_FIX_ALTITUDE
            const PROGMEM char* gpsUpSpeedString = "gpsSpeedUp(m/s)";
            includeVariableInPackage(gpsUpSpeedString, PMM_TELEMETRY_TYPE_FLOAT, &pmmGpsStruct->upSpeed);
        #endif
    #endif*/
}





void PmmPackageLog::addCustomVariable(const char* variableName, uint8_t variableType, void* variableAddress)
{
    includeVariableInPackage(variableName, variableType, variableAddress);
}

uint8_t PmmPackageLog::getNumberOfVariables()
{
    return mActualNumberVariables;
}

uint8_t PmmPackageLog::getPackageSizeInBytes()
{
    return mPackageSizeInBytes;
}

const char** PmmPackageLog::getVariableNameArray()  { return mVariableNameArray;}
uint8_t* PmmPackageLog::getVariableTypeArray()      { return mVariableTypeArray;}
uint8_t* PmmPackageLog::getVariableSizeArray()      { return mVariableSizeArray;}
uint8_t** PmmPackageLog::getVariableAddressArray()     { return mVariableAddressArray;}



#if PMM_DEBUG_SERIAL
// Note for the 2 functions below:
// There are faster ways to print the debugs, but since it isn't something that is going to be used frequently,
// I (HB :) ) will spend my precious time on other stuffs)

void PmmPackageLog::debugPrintLogHeader()
{
    unsigned variableIndex;
    char buffer[512] = {0}; // No static needed, as it is called only once.

    if (mActualNumberVariables > PMM_PACKAGE_LOG_DATA_INDEX)
        snprintf(buffer, 512, "%s", mVariableNameArray[PMM_PACKAGE_LOG_DATA_INDEX]);

    for (variableIndex = PMM_PACKAGE_LOG_DATA_INDEX + 1; variableIndex < mActualNumberVariables; variableIndex ++)
    {
        snprintf(buffer, 512, "%s | %s", buffer, mVariableNameArray[variableIndex]);
    }
    Serial.println(buffer);
}



void PmmPackageLog::debugPrintLogContent()
{
    unsigned variableIndex;
    static char buffer[512]; // Static for optimization
    buffer[0] = '\0';
    for (variableIndex = 0; variableIndex < mActualNumberVariables; variableIndex ++)
    {
        switch(mVariableTypeArray[variableIndex])
        {
            case PMM_TELEMETRY_TYPE_FLOAT: // first as it is more common
                snprintf(buffer, 512, "%s%f, ", buffer, *(float*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT32:
                snprintf(buffer, 512, "%s%lu, ", buffer, *(uint32_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT32:
                snprintf(buffer, 512, "%s%li, ", buffer, *(int32_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT8:
                snprintf(buffer, 512, "%s%u, ", buffer, *(uint8_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT8:
                snprintf(buffer, 512, "%s%i, ", buffer, *(int8_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT16:
                snprintf(buffer, 512, "%s%u, ", buffer, *(uint16_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT16:
                snprintf(buffer, 512, "%s%i, ", buffer, *(int16_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_UINT64:
                snprintf(buffer, 512, "%s%llu, ", buffer, *(uint64_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_INT64:
                snprintf(buffer, 512, "%s%lli, ", buffer, *(int64_t*) (mVariableAddressArray[variableIndex]));
                break;
            case PMM_TELEMETRY_TYPE_DOUBLE:
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
