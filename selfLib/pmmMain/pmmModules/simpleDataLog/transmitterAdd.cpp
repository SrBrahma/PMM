#include "pmmDebug.h"
#include "pmmModules/simpleDataLog/transmitter.h"

int ModuleSimpleDataLogTx::addBasicInfo(uint32_t* mainLoopCounter, uint32_t* timeMs) {
    int returnValue;
    if ((returnValue = includeVariable(mStr.transmissionCounter, TYPE_ID_UINT32, &mTxCounter    ))) return returnValue;
    if ((returnValue = includeVariable(mStr.mainLoopCounter,     TYPE_ID_UINT32, mainLoopCounter))) return returnValue;
    if ((returnValue = includeVariable(mStr.timeMs,          TYPE_ID_UINT32, timeMs     ))) return returnValue;
    return 0;
}

int ModuleSimpleDataLogTx::addAccelerometer    (float  array[] ) {
    int returnValue;
    if ((returnValue = includeVariable(mStr.accelerometerX, TYPE_ID_FLOAT, &array[0])));
    if ((returnValue = includeVariable(mStr.accelerometerY, TYPE_ID_FLOAT, &array[1])));
    if ((returnValue = includeVariable(mStr.accelerometerZ, TYPE_ID_FLOAT, &array[2])));
    return 0;
}
int ModuleSimpleDataLogTx::addGyroscope        (float  array[] ) {
    int returnValue;
    if ((returnValue = includeVariable(mStr.gyroscopeX, TYPE_ID_FLOAT, &array[0])));
    if ((returnValue = includeVariable(mStr.gyroscopeY, TYPE_ID_FLOAT, &array[1])));
    if ((returnValue = includeVariable(mStr.gyroscopeZ, TYPE_ID_FLOAT, &array[2])));
    return 0;
}
int ModuleSimpleDataLogTx::addMagnetometer     (float  array[] ) {
    int returnValue;
    if ((returnValue = includeVariable(mStr.gyroscopeX, TYPE_ID_FLOAT, &array[0])));
    if ((returnValue = includeVariable(mStr.gyroscopeY, TYPE_ID_FLOAT, &array[1])));
    if ((returnValue = includeVariable(mStr.gyroscopeZ, TYPE_ID_FLOAT, &array[2])));
    return 0;
}
int ModuleSimpleDataLogTx::addMpuTemperature   (float *mpuTemp ) { return includeVariable(mStr.mpuTemperature,    TYPE_ID_FLOAT, mpuTemp ); }

int ModuleSimpleDataLogTx::addBarometerPressure(float *barPress) { return includeVariable(mStr.barometerPressure, TYPE_ID_FLOAT, barPress); }
int ModuleSimpleDataLogTx::addBarometerAltitude(float *altitude) { return includeVariable(mStr.barometerAltitude, TYPE_ID_FLOAT, altitude); }
int ModuleSimpleDataLogTx::addBmpTemperature   (float *barTemp ) { return includeVariable(mStr.bmpTemperature,    TYPE_ID_FLOAT, barTemp ); }

int  ModuleSimpleDataLogTx::addGpsLatLong(int32_t *latitude, int32_t *longitude) {
    int returnVal;
    if ((returnVal = includeVariable(mStr.gpsLatitude,  TYPE_ID_INT32, latitude ))) return returnVal;
    if ((returnVal = includeVariable(mStr.gpsLongitude, TYPE_ID_INT32, longitude))) return returnVal;
    return 0;
}

int  ModuleSimpleDataLogTx::addGpsAltitude          (float   *altitude   ) { return includeVariable(mStr.gpsAltitude,           TYPE_ID_FLOAT, altitude);    }
int  ModuleSimpleDataLogTx::addGpsSatellites        (uint8_t *satellites ) { return includeVariable(mStr.gpsSatellites,         TYPE_ID_UINT8, satellites);  }
int  ModuleSimpleDataLogTx::addGpsLastLocationTimeMs(uint32_t *lastTimeMs) { return includeVariable(mStr.gpsLastLocationTimeMs, TYPE_ID_UINT32, lastTimeMs); }




int ModuleSimpleDataLogTx::addCustomVariable(const char* variableName, uint8_t variableType, void* variableAddress) {
    return includeVariable(variableName, variableType, variableAddress);
}

int ModuleSimpleDataLogTx::includeVariable(const char *variableName, uint8_t variableType, void *variableAddress)
{
    if (!variableName)    return 1;
    if (!variableAddress) return 2;

    uint8_t varSize = getVarSize(variableType);

    if (mNumberVariables >= MAX_VARIABLES) {
        advPrintf("Failed to add the variable \"%s\". Exceeds the maximum number of variables in the DataLog.", variableName)
        return 3;
    }

    if ((mTotalBytes + varSize) >= TLM_MAX_PAYLOAD_LENGTH) {
        advPrintf("Failed to add the variable \"%s\". Exceeds the maximum content byte size (tried to be %u, max is %u).", variableName, mTotalBytes + varSize, TLM_MAX_PAYLOAD_LENGTH)
        return 4;
    }

    mVarsNameArray[mNumberVariables] = (char*) variableName; // Typecast from (const char*) to (char*)
    mVarsTypeArray[mNumberVariables] = variableType;
    mVarsSizeArray[mNumberVariables] = varSize;
    mVarsAdrsArray[mNumberVariables] = (uint8_t*) variableAddress;
    mNumberVariables++;
    mTotalBytes += varSize;

    return 0;
}


// int ModuleSimpleDataLogTx::includeArray(const char* const* variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize)
// {
//     if (!variableName) return 1;
//     if (!arrayAddress) return 2;

//     int returnVal;
//     for (int counter = 0; counter < arraySize; counter++)
//         if ((returnVal = includeVariable(*variableName++, arrayType, (uint8_t*) arrayAddress + (getVarSize(arrayType) * counter))))
//             return returnVal;

//     return 0;
// }