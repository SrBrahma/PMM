/* dataLog.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_DATA_LOG_h
#define PMM_MODULE_DATA_LOG_h

#include "pmmTelemetry/pmmTelemetry.h"      // For transmitting
#include "pmmSd/pmmSd.h"                    // For storing
#include "pmmModules/dataLog/dataLogGroupCore.h"
#include "pmmModules/dataLog/dataLogInfo/logInfo.h" // For specific defines


// DataLog Defines
#define PORT_DATA_LOG_INDEX_CRC_8_HEADER            0
#define PORT_DATA_LOG_INDEX_SESSION_ID              1
#define PORT_DATA_LOG_INDEX_DATA_LOG_ID             2
#define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB      3
#define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB      4
// Total header length is equal to...
#define PORT_DATA_LOG_HEADER_LENGTH                 5

#define PORT_DATA_LOG_PAYLOAD_START                 PORT_DATA_LOG_HEADER_LENGTH

#define PORT_DATA_LOG_MAX_PAYLOAD_LENGTH            (PMM_TLM_MAX_PAYLOAD_LENGTH - PORT_DATA_LOG_HEADER_LENGTH)



class PmmModuleDataLog : public PmmModuleDataLogGroupCore
{

public:

    PmmModuleDataLog();

    int  init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* packageId, uint32_t* mainMillisPtr, bool createGroup = true);
    int  update();   // Will automatically sendDataLog, sendDataLogInfo and store on the memories.

    int  createGroup(uint8_t* returnGroupId);
    int  setSystemMode(pmmSystemState systemMode);

    int  getGroup(
    // Reception
    int  receivedDataLog(receivedPacketAllInfoStructType* packetInfo);
    int  receivedDataLogInfo(receivedPacketAllInfoStructType* packetInfo);

    // Debug!
    void debugPrintLogHeader ();
    void debugPrintLogContent();


private:

    // Transmission
    int  sendDataLog(uint8_t destinationAddress = PMM_TLM_ADDRESS_BROADCAST, telemetryQueuePriorities priority = PMM_TLM_QUEUE_PRIORITY_LOW);
    int  sendDataLogInfo(uint8_t requestedPacket, uint8_t destinationAddress = PMM_TLM_ADDRESS_BROADCAST, telemetryQueuePriorities priority = PMM_TLM_QUEUE_PRIORITY_NORMAL);

    // Build the LogInfo
    void updateLogInfoCombinedPayload(); // Updates the DataLogInfo

    // Storage
    int  getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[] = NULL);
    
    int  saveDataLog        (uint8_t groupData[], char dirRelativePath[], PmmSdAllocStatus* allocStatus);
    int  saveOwnDataLog     ();
    int  saveReceivedDataLog(uint8_t groupData[], uint8_t groupLength, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession);

    // Variables
    PmmTelemetry*  mPmmTelemetry;
    PmmSd       *  mPmmSd;
    PmmSdSafeLog*  mPmmSdSafeLog;

    pmmSystemState mSystemMode;

    // Update
    uint8_t  mUpdateModeReadyCounter, mUpdateModeDeployedCounter;
    uint8_t  mUpdateDataLogInfoCounter;

    // Storage reception
    uint8_t  mGroupTempData[PORT_DATA_LOG_MAX_PAYLOAD_LENGTH];  // Used in the saveOwnDataLog(). This, however, isn't used in the temeletry.
    
    static constexpr const char* LOG_INFO_FILENAME PROGMEM = "DataLogInfo"; // https://stackoverflow.com/a/25323360/10247962

    PmmSdAllocStatus mAllocStatusReceived       [PMM_TLM_ADDRESSES_FINAL_ALLOWED_SOURCE];
    uint8_t          mAllocStatusReceivedSession[PMM_TLM_ADDRESSES_FINAL_ALLOWED_SOURCE];

    // Storage self
    PmmSdAllocStatus mAllocStatusSelfDataLog;

    char   mDataLogSelfDirPath[PMM_SD_FILENAME_MAX_LENGTH];

}; // End of the class

#endif
