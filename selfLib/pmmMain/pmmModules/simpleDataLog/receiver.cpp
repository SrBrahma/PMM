/* PmmPortLogReception.cpp
 * Defines the Package Log (MLOG) and the Package Log Information (MLIN).
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#include <byteSelection.h>  // for join2Bytes. yeah, really!
#include <crc.h.>
#include "pmmDebug.h"
#include "pmmModules/simpleDataLog/core.h"
#include "pmmModules/simpleDataLog/receiver.h"

ModuleSimpleDataLogRx::ModuleSimpleDataLogRx() {}

int  ModuleSimpleDataLogRx::init(PmmSd* pmmSd, uint8_t systemSession, uint8_t sourceAddree) { 
    mPmmSdPtr      = pmmSd;
    mSessionId     = systemSession;
    mSourceAddress = sourceAddree;
    return 0;
}



int  ModuleSimpleDataLogRx::getVarIndex(char varExactName[]) {
    for (int i = 0; i < mNumberVariables; i++)
        if (strcmp(varExactName, mVarsNameArray[i]) == 0)
            return i;
    return -1;
}
int ModuleSimpleDataLogRx::getVarByIndex(void *destination, int index) {
    if (!destination)                           return -1;
    if (index < 0 || index >= mNumberVariables) return -2;
    memcpy(destination, mVarsAdrsArray[index], mVarsSizeArray[index]);
}



// Received Package Log Info Package
bool ModuleSimpleDataLogRx::receivedPacket(receivedPacketAllInfoStructType* packet, bool autoStoreOnSd = false)
{
    if (!packet)                                 return false;
    if (packet->payloadLength < 1)               return false;
    if (packet->sourceAddress != mSourceAddress) return false;

    // 1) First, check the CRC-16 of this packet.
    uint16_t receivedCrc = join2Bytes(packet->payload[TLM_INDEX_CRC_16_MSB], packet->payload[TLM_INDEX_CRC_16_LSB]);

    if (crc16(packet->payload + TLM_CRC_LENGTH, packet->payloadLength - TLM_CRC_LENGTH) != receivedCrc)
        return false; // Ignore this packet

    memcpy(mVarsData, packet->payload + TLM_HEADER_LENGTH, min(packet->payloadLength, TLM_MAX_PAYLOAD_LENGTH));
    storeOnSd(packet->payload[TLM_INDEX_SESSION_ID]);

    return true;
}



int  ModuleSimpleDataLogRx::storeOnSd(uint8_t sourceSession, bool writeOnBckupToo) {
    #define dataMaxLength 1024
    char data[dataMaxLength];
    File file, file2;
    char filePath[PMM_SD_FILENAME_MAX_LENGTH] = {'\0'};
    char filePath2[PMM_SD_FILENAME_MAX_LENGTH] = {'\0'};

    mPmmSdPtr->getReceivedDirectory(filePath2, PMM_SD_FILENAME_MAX_LENGTH, mSourceAddress, sourceSession);
    snprintf(filePath, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", filePath2, "/simpleDataLog.csv");
    if (writeOnBckupToo)
        snprintf(filePath2, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", filePath2, "/simpleDataLogBckup.csv");

    if (!mPmmSdPtr->exists(filePath)) {
        buildCsvHeader(data, dataMaxLength);
        mPmmSdPtr->createDirsAndOpen(&file, filePath);
        file.print(data);
        
        if (writeOnBckupToo) {
            mPmmSdPtr->createDirsAndOpen(&file2, filePath2);
            file2.print(data);
        }
    }

    buildCsvData(data, dataMaxLength);

    file.print(data);  file.close();
    if (writeOnBckupToo)
        file2.print(data); file2.close();

    return 0;
}



int ModuleSimpleDataLogRx::addBasicInfo() {
    int returnValue;
    if ((returnValue = includeVariable(mStr.transmissionCounter, TYPE_ID_UINT32))) return returnValue;
    if ((returnValue = includeVariable(mStr.mainLoopCounter,     TYPE_ID_UINT32))) return returnValue;
    if ((returnValue = includeVariable(mStr.timeMillis,          TYPE_ID_UINT32))) return returnValue;
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
    if ((returnVal = includeVariable(mStr.gpsLatitude,  TYPE_ID_UINT32))) return returnVal;
    if ((returnVal = includeVariable(mStr.gpsLongitude, TYPE_ID_UINT32))) return returnVal;
    return 0;
}

int  ModuleSimpleDataLogRx::addGpsAltitude() { return includeVariable(mStr.gpsAltitude,   TYPE_ID_FLOAT);}
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