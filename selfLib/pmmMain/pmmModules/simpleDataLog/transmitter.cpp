#include <crc.h>
#include <byteSelection.h>

#include "pmmDebug.h"                               // For advPrintf().
#include "pmmModules/ports.h"
#include "pmmModules/simpleDataLog/core.h"
#include "pmmModules/simpleDataLog/transmitter.h"

ModuleSimpleDataLogTx::ModuleSimpleDataLogTx() {}

int  ModuleSimpleDataLogTx::init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession) {
    mPmmTelemetryPtr = pmmTelemetry; mPmmSdPtr = pmmSd; mSystemSession = systemSession;
    mIsFirstStoreOnSd = true;
    return 0;
}



// ---------- SimpleDataLog Header 1.0 ---------------
// [Positions] : [Function] : [ Length in Bytes ]
//
// a) [0~1] : [ CRC-16 of this payload . ] : [ 2 ]
// b) [ 2 ] : [ Session Identifier ..... ] : [ 1 ]
// 
//                    Total header length = 3 bytes.
// --------------------------------------------------

int ModuleSimpleDataLogTx::send(uint8_t destinationAddress)
{
    if (!mPmmTelemetryPtr->isSendAvailable())   return 1; // Won't waste time building the packet.

    PacketToBeSent packet;
    uint8_t payloadLength;

    // 1) Add the Session Identifier. The CRC16 is added on 3).
    packet.payload[TLM_INDEX_SESSION_ID]  = mSystemSession;
    payloadLength = TLM_HEADER_LENGTH;

    // 2) Add the Log data.
    for (int i = 0; i < mNumberVariables; i++) {
        memcpy(packet.payload + payloadLength, mVarsAdrsArray[i], mVarsSizeArray[i]);
        payloadLength += mVarsSizeArray[i];
    }

    // 3) Add the CRC-16 of the log to the Header (1.4)
    uint16_t crcValue = crc16(packet.payload + TLM_CRC_LENGTH, payloadLength - TLM_CRC_LENGTH);
    packet.payload[TLM_INDEX_CRC_16_LSB] = LSB0(crcValue);
    packet.payload[TLM_INDEX_CRC_16_MSB] = LSB1(crcValue);

    // 5) Add the remaining fields and add it to the queue!
    packet.addInfo(PMM_NEO_PROTOCOL_ID, PMM_TLM_ADDRESS_THIS_SYSTEM, destinationAddress, PORT_ID_SIMPLE_DATA_LOG, payloadLength);
    
    if (mPmmTelemetryPtr->sendIfAvailable(&packet) == 0)    // If successfully sent,
        mTxCounter++;                                       // Increase the transmission counter.
    return 0;
}



int  ModuleSimpleDataLogTx::storeOnSd(bool writeOnBckupToo)
{
    #define dataMaxLength 1024
    char data[dataMaxLength];

    if (mIsFirstStoreOnSd) {
        mPmmSdPtr->getSelfDirectory(mFilePath, PMM_SD_FILENAME_MAX_LENGTH);

        snprintf(mFilePath, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", mFilePath, "/simpleDataLog.csv");
        if (writeOnBckupToo)
            snprintf(mFilePath2, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", mFilePath2, "/simpleDataLogBckup.csv");

        mPmmSdPtr->createDirsAndOpen(&mFile, mFilePath);
        
        buildCsvHeader(data, dataMaxLength);
        mFile.print(data);
        if (writeOnBckupToo) {
            mPmmSdPtr->createDirsAndOpen(&mFile2, mFilePath2);
            mFile2.print(data);
        }
        mIsFirstStoreOnSd = false;
    }

    buildCsvData(data, dataMaxLength);
    mFile.print(data);  mFile.flush();
    if (writeOnBckupToo)
        mFile2.print(data);  mFile2.flush();

    return 0;
}



#define DEBUG_BUFFER_LEN 2048

void ModuleSimpleDataLogTx::printHeader()
{
    char buffer[DEBUG_BUFFER_LEN] = {'\0'}; // No static needed, as it is called usually only once.

    for (int i = 0; i < mNumberVariables; i++) {
        if (i > 0)
            snprintf(buffer, DEBUG_BUFFER_LEN, "%s ", buffer);
        snprintf(buffer, DEBUG_BUFFER_LEN, "%s[%u) %s", buffer, i, mVarsNameArray[i]);
        snprintf(buffer, DEBUG_BUFFER_LEN, "%s]", buffer);
    }
    Serial.println(buffer);
}

void ModuleSimpleDataLogTx::printContent(bool printHeadersTogether)
{
    #if PMM_DEBUG && PMM_DATA_LOG_DEBUG
    static char buffer[DEBUG_BUFFER_LEN]; // Static for optimization

    strcpy(buffer, "<");    // Zero the string, with a symbolic header.

    for (int i = 0; i < mDataLogGroupCore.getNumberOfVariables(); i++)
    {
        if (i > 0)
            snprintf(buffer, DEBUG_BUFFER_LEN, "%s ", buffer);

        // Print the index.
        snprintf(buffer, DEBUG_BUFFER_LEN, "%s[%u) ", buffer, i);

        // Print the header.
        if (printHeadersTogether)
            snprintf(buffer, DEBUG_BUFFER_LEN, "%s%s: ", buffer, mDataLogGroupCore.getVarNameArray()[i]);

        switch(mDataLogGroupCore.getVarTypeArray()[i])
        {
            case TYPE_ID_FLOAT: // first as it is more common
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%.2f", buffer, *(float*)    (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_UINT32:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%lu",  buffer, *(uint32_t*) (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_INT32:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%li",  buffer, *(int32_t*)  (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_UINT8:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%u",   buffer, *(uint8_t*)  (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_INT8:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%i",   buffer, *(int8_t*)   (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_UINT16:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%u",   buffer, *(uint16_t*) (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_INT16:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%i",   buffer, *(int16_t*)  (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_UINT64:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%llu", buffer, *(uint64_t*) (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_INT64:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%lli", buffer, *(int64_t*)  (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            case TYPE_ID_DOUBLE:
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%.2f", buffer, *(double*)   (mDataLogGroupCore.getVarAdrsArray()[i])); break;
            default:    // If none above,
                snprintf(buffer, DEBUG_BUFFER_LEN, "%s%s",   buffer, "ERROR HERE!"); break;
        } // switch end
        snprintf(buffer, DEBUG_BUFFER_LEN, "%s]", buffer);
    } // for loop end
    snprintf(buffer, DEBUG_BUFFER_LEN, "%s>\n", buffer);
    Serial.println(buffer);

    #endif
} // end of function debugPrintLogContent()






int ModuleSimpleDataLogTx::addBasicInfo(uint32_t* mainLoopCounter, uint32_t* timeMillis) {
    int returnValue;
    if ((returnValue = includeVariable(mStr.transmissionCounter, TYPE_ID_UINT32, &mTxCounter    ))) return returnValue;
    if ((returnValue = includeVariable(mStr.mainLoopCounter,     TYPE_ID_UINT32, mainLoopCounter))) return returnValue;
    if ((returnValue = includeVariable(mStr.timeMillis,          TYPE_ID_UINT32, timeMillis     ))) return returnValue;
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

int  ModuleSimpleDataLogTx::addGpsLatLong(uint32_t *latitude, uint32_t *longitude) {
    int returnVal;
    if ((returnVal = includeVariable(mStr.gpsLatitude,  TYPE_ID_UINT32, latitude ))) return returnVal;
    if ((returnVal = includeVariable(mStr.gpsLongitude, TYPE_ID_UINT32, longitude))) return returnVal;
    return 0;
}

int  ModuleSimpleDataLogTx::addGpsAltitude     (float   *altitude  ) { return includeVariable(mStr.gpsAltitude,   TYPE_ID_FLOAT, altitude);}
int  ModuleSimpleDataLogTx::addGpsSatellites   (uint8_t *satellites) { return includeVariable(mStr.gpsSatellites, TYPE_ID_UINT8, satellites); }





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