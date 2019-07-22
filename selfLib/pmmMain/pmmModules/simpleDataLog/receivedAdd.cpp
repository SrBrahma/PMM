// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#include "pmmDebug.h"
#include "pmmModules/simpleDataLog/receiver.h"


int ModuleSimpleDataLogRx::addBasicInfo() {
    int returnValue;
    if ((returnValue = includeVariable(mStr.transmissionCounter, TYPE_ID_UINT32))) return returnValue;
    if ((returnValue = includeVariable(mStr.mainLoopCounter,     TYPE_ID_UINT32))) return returnValue;
    if ((returnValue = includeVariable(mStr.timeMs,              TYPE_ID_UINT32))) return returnValue;
    return 0;
}

int ModuleSimpleDataLogRx::addAccelerometer() {
    int returnValue;
    if ((returnValue = includeVariable(mStr.accelerometerX, TYPE_ID_FLOAT)));
    if ((returnValue = includeVariable(mStr.accelerometerY, TYPE_ID_FLOAT)));
    if ((returnValue = includeVariable(mStr.accelerometerZ, TYPE_ID_FLOAT)));
    return 0;
}
int ModuleSimpleDataLogRx::addGyroscope() {
    int returnValue;
    if ((returnValue = includeVariable(mStr.gyroscopeX, TYPE_ID_FLOAT)));
    if ((returnValue = includeVariable(mStr.gyroscopeY, TYPE_ID_FLOAT)));
    if ((returnValue = includeVariable(mStr.gyroscopeZ, TYPE_ID_FLOAT)));
    return 0;
}
int ModuleSimpleDataLogRx::addMagnetometer() {
    int returnValue;
    if ((returnValue = includeVariable(mStr.gyroscopeX, TYPE_ID_FLOAT)));
    if ((returnValue = includeVariable(mStr.gyroscopeY, TYPE_ID_FLOAT)));
    if ((returnValue = includeVariable(mStr.gyroscopeZ, TYPE_ID_FLOAT)));
    return 0;
}
int ModuleSimpleDataLogRx::addMpuTemperature() { return includeVariable(mStr.mpuTemperature,    TYPE_ID_FLOAT); }

int ModuleSimpleDataLogRx::addBarometerPressure() { return includeVariable(mStr.barometerPressure, TYPE_ID_FLOAT); }
int ModuleSimpleDataLogRx::addBarometerAltitude() { return includeVariable(mStr.barometerAltitude, TYPE_ID_FLOAT); }
int ModuleSimpleDataLogRx::addBmpTemperature   () { return includeVariable(mStr.bmpTemperature,    TYPE_ID_FLOAT); }

int  ModuleSimpleDataLogRx::addGpsLatLong() {
    int returnVal;
    if ((returnVal = includeVariable(mStr.gpsLatitude,  TYPE_ID_INT32))) return returnVal;
    if ((returnVal = includeVariable(mStr.gpsLongitude, TYPE_ID_INT32))) return returnVal;
    return 0;
}

int  ModuleSimpleDataLogRx::addGpsLastLocationTimeMs() { return includeVariable(mStr.gpsLastLocationTimeMs, TYPE_ID_UINT32); }


int  ModuleSimpleDataLogRx::addGpsAltitude()   { return includeVariable(mStr.gpsAltitude,   TYPE_ID_FLOAT);}
int  ModuleSimpleDataLogRx::addGpsSatellites() { return includeVariable(mStr.gpsSatellites, TYPE_ID_UINT8); }



int ModuleSimpleDataLogRx::addCustomVariable(const char* variableName, uint8_t variableType) {
    return includeVariable(variableName, variableType);
}

int ModuleSimpleDataLogRx::includeVariable(const char *variableName, uint8_t variableType)
{
    if (!variableName)    return 1;

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
    mVarsAdrsArray[mNumberVariables] = mVarsData + mTotalBytes;

    mNumberVariables++;

    mTotalBytes += varSize;

    return 0;
}