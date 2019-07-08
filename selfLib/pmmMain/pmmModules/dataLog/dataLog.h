/* dataLog.h
 * This class handles the received the dataLog and dataLogInfo packets, and the groupCores.
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_DATA_LOG_h
#define PMM_MODULE_DATA_LOG_h

#include "pmmTelemetry/telemetry.h"                 // For transmitting
#include "pmmSd/sd.h"                               // For storing
#include "pmmModules/dataLog/dataLogGroupCore.h"
#include "pmmModules/dataLog/dataLogInfo/logInfo.h" // For specific defines


#define PORT_DATA_LOG_INDEX_CRC_8_HEADER            0
#define PORT_DATA_LOG_INDEX_SESSION_ID              1
#define PORT_DATA_LOG_INDEX_DATA_LOG_ID             2
#define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB      3
#define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB      4
// Total header length is equal to...
#define PORT_DATA_LOG_HEADER_LENGTH                 5

#define PORT_DATA_LOG_PAYLOAD_START                 PORT_DATA_LOG_HEADER_LENGTH

#define PORT_DATA_LOG_MAX_PAYLOAD_LENGTH            (PMM_TLM_MAX_PAYLOAD_LENGTH - PORT_DATA_LOG_HEADER_LENGTH)



class PmmModuleDataLog
{

public:

    PmmModuleDataLog();

    int  init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* mainLoopCounterPtr, uint32_t* timeMillisPtr);
    int  update();   // Will automatically sendDataLog, sendDataLogInfo and store on the memories.



    // Reception
    int  receivedDataLog(receivedPacketAllInfoStructType* packetInfo);
    int  receivedDataLogInfo(receivedPacketAllInfoStructType* packetInfo);

    // Debug!
    void debugPrintLogHeader ();
    void debugPrintLogContent();

    PmmModuleDataLogGroupCore* getDataLogGroupCore(uint8_t dataLogGroupId = 0); // The argument for now is useless. You can imagine what it's for.

private:

    int  saveReceivedDataLog(uint8_t groupData[], uint8_t groupLength, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession);
    int  saveReceivedDataLogInfo(uint8_t data[], uint16_t dataLength, uint8_t sourceAddress, uint8_t sourceSession, uint8_t dataLogId, uint8_t groupLength, uint8_t currentPart, uint8_t totalParts);

    PmmModuleDataLogGroupCore mDataLogGroupCore;

    PmmTelemetry*  mPmmTelemetryPtr;
    PmmSd       *  mPmmSdPtr;
    PmmSdSafeLog*  mPmmSdSafeLogPtr;

    uint8_t  mSystemSession;

    // Update
    uint8_t  mUpdateModeReadyCounter, mUpdateModeDeployedCounter;
    uint8_t  mUpdateDataLogInfoCounter;

    PmmSdAllocStatus mAllocStatusReceived       [PMM_TLM_ADDRESSES_FINAL_ALLOWED_SOURCE];
    uint8_t          mAllocStatusReceivedSession[PMM_TLM_ADDRESSES_FINAL_ALLOWED_SOURCE];



};

#endif
