// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#include <byteSelection.h>  // for join2Bytes. yeah, really!
#include <crc.h>
#include "pmmDebug.h"
#include "pmmModules/simpleDataLog/receiver.h"

ModuleSimpleDataLogRx::ModuleSimpleDataLogRx() {}

int  ModuleSimpleDataLogRx::init(PmmSd* pmmSd, uint8_t systemSession, uint8_t sourceAddress) { 
    mPmmSdPtr      = pmmSd;
    mSessionId     = systemSession;
    mSourceAddress = sourceAddress;
    memset(mVarsData, 0, TLM_MAX_PAYLOAD_LENGTH); // Zero the array.
    return 0;
}



int  ModuleSimpleDataLogRx::getVarIndex(const char varExactName[]) {
    for (int i = 0; i < mNumberVariables; i++)
        if (strcmp(varExactName, mVarsNameArray[i]) == 0)
            return i;
    return -1;
}
int ModuleSimpleDataLogRx::getVarByIndex(void *destination, int index) {
    if (!destination)                           return -1;
    if (index < 0 || index >= mNumberVariables) return -2;
    memcpy(destination, mVarsAdrsArray[index], mVarsSizeArray[index]);
    return 0;
}



// Received Package Log Info Package
bool ModuleSimpleDataLogRx::receivedPacket(receivedPacketAllInfoStructType* packet, bool autoStoreOnSd)
{
    if (!packet)                                 return false;
    if (packet->payloadLength < 3)               return false; // Should at least have the 2 bytes of crc16, and the session.
    if (packet->sourceAddress != mSourceAddress) return false;

    // 1) First, check the CRC-16 of this packet.
    uint16_t receivedCrc = join2Bytes(packet->payload[TLM_INDEX_CRC_16_MSB], packet->payload[TLM_INDEX_CRC_16_LSB]);

    if (crc16(packet->payload + TLM_CRC_LENGTH, packet->payloadLength - TLM_CRC_LENGTH) != receivedCrc)
        return false; // Ignore this packet

    mSourceSession = packet->payload[TLM_INDEX_SESSION_ID];

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

uint8_t  ModuleSimpleDataLogRx::getVar_uint8_t (int index)  {
    if (index < 0 || index > mNumberVariables) return 0;
    return  *(uint8_t*) (mVarsAdrsArray[index]);
}
int32_t  ModuleSimpleDataLogRx::getVar_int32_t (int index)  {
    if (index < 0 || index > mNumberVariables) return 0;
    return  *(int32_t*) (mVarsAdrsArray[index]);
}
uint32_t ModuleSimpleDataLogRx::getVar_uint32_t(int index)  {
    if (index < 0 || index > mNumberVariables) return 0;
    return  *(uint32_t*) (mVarsAdrsArray[index]);
}
float    ModuleSimpleDataLogRx::getVar_float   (int index)  {
    if (index < 0 || index > mNumberVariables) return 0;
    return  *(float*) (mVarsAdrsArray[index]);
}

uint8_t ModuleSimpleDataLogRx::getSourceSession() { return mSourceSession; }
