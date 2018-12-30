/* PmmModuleDataLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_DATA_LOG_h
#define PMM_MODULE_DATA_LOG_h

#include "pmmTelemetry/pmmTelemetry.h"      // For transmitting
#include "pmmSd/pmmSd.h"                    // For storing
#include "pmmGps/pmmGps.h"                  // For GPS struct
#include "pmmImu/pmmImu.h"                  // For IMU struct
#include "pmmModules/dataLogInfo/logInfo.h" // For specific defines


// 000X 0-1 (1 byte)
#define MODULE_DATA_LOG_TYPE_UINT8      0
#define MODULE_DATA_LOG_TYPE_INT8       1

// 00X0 2-3 (2 bytes)
#define MODULE_DATA_LOG_TYPE_UINT16     2
#define MODULE_DATA_LOG_TYPE_INT16      3

// 0X00 4-8 (4 bytes)
#define MODULE_DATA_LOG_TYPE_UINT32     4
#define MODULE_DATA_LOG_TYPE_INT32      5
#define MODULE_DATA_LOG_TYPE_FLOAT      6

// X000 8-15 (8 bytes)
#define MODULE_DATA_LOG_TYPE_INT64      8
#define MODULE_DATA_LOG_TYPE_UINT64     9
#define MODULE_DATA_LOG_TYPE_DOUBLE     10



// DataLog AND DataLogInfo Defines (Which I will call as DATA_LOG)
#define MODULE_DATA_LOG_MAX_VARIABLES               50  // This must be the same value for the transmitter and the receptor.
#define MODULE_DATA_LOG_MAX_STRING_LENGTH           30  // The maximum Variable String. Includes the '\0', so the max valid chars is this - 1.


// DataLog Defines
#define PORT_DATA_LOG_INDEX_CRC_8_HEADER            0
#define PORT_DATA_LOG_INDEX_SESSION_ID              1
#define PORT_DATA_LOG_INDEX_DATA_LOG_ID             2
#define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB      3
#define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB      4
// Total header length is equal to...
#define PORT_DATA_LOG_HEADER_LENGTH                 5

#define PORT_DATA_LOG_PAYLOAD_START                 PORT_DATA_LOG_HEADER_LENGTH

#define PORT_DATA_LOG_MAX_PAYLOAD_LENGTH            (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PORT_DATA_LOG_HEADER_LENGTH)



class PmmModuleDataLog // Intended to have >1 Objects of this class, one for each DataLog Package
{

public:

    PmmModuleDataLog();

    int  init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* packageId, uint32_t* mainMillisPtr);
    int  update();   // Will automatically sendDataLog, sendDataLogInfo and store on the memories.

    int  setSystemMode(pmmSystemState systemMode);


// Reception
    int  receivedDataLog(receivedPacketAllInfoStructType* packetInfo);
    int  receivedLogInfo(receivedPacketAllInfoStructType* packetInfo);


// Add variables to DataLog. Their types are specified in PmmModuleDataLog.cpp.
    int  addMagnetometer        (void* magnetometerArray );
    int  addGyroscope           (void* gyroscopeArray    );
    int  addAccelerometer       (void* accelerometerArray);
    int  addTemperatureMpu      (void* mpuTemperaturePtr );
    int  addBarometer           (void* barometerPtr      );
    int  addRawAltitudeBarometer(void* rawAltitudePressure); // Without filtering
    int  addAltitudeBarometer   (void* altitudePtr       );
    int  addTemperatureBmp(void* barometerTempPtr  );

    int  addImu(pmmImuStructType* pmmImuStructPtr); // Adds all the sensors above.
    int  addGps(pmmGpsStructType* pmmGpsStruct   );

    // For a quick way to add a variable to the package. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like MODULE_DATA_LOG_TYPE_UINT8;
    int  addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);


    // Getters
    uint8_t      getNumberOfVariables();
    const char** getVariableNameArray();
    uint8_t*     getVariableTypeArray();
    uint8_t*     getVariableSizeArray();
    uint8_t**    getVariableAdrsArray();

    // Debug!
    void debugPrintLogHeader ();
    void debugPrintLogContent();


private:

// Transmission
    int  sendDataLog(uint8_t destinationAddress = PMM_TELEMETRY_ADDRESS_BROADCAST, telemetryQueuePriorities priority = PMM_TELEMETRY_QUEUE_PRIORITY_LOW);
    int  sendDataLogInfo(uint8_t requestedPacket, uint8_t destinationAddress = PMM_TELEMETRY_ADDRESS_BROADCAST, telemetryQueuePriorities priority = PMM_TELEMETRY_QUEUE_PRIORITY_NORMAL);

// Auxiliar functions
    uint8_t variableTypeToVariableSize(uint8_t variableType);

// Add variables to the Data Log. The types are specified in PmmModuleDataLog.cpp.
    int  addBasicInfo(uint32_t* packageId, uint32_t* packageTimeMs);

    int  includeVariableInPackage(const char*  variableName,   uint8_t variableType, void* variableAddress);
    int  includeArrayInPackage   (const char** variablesNames, uint8_t arrayType,    void* arrayAddress, uint8_t arraySize);

// Build the Package Log Info Package
    void updateLogInfoCombinedPayload(); // Updates the DataLogInfo

// Storage
    int  getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[] = NULL);
    
    int  saveDataLog        (uint8_t groupData[], char dirRelativePath[], PmmSdAllocStatus* allocStatus);
    int  saveOwnDataLog     ();
    int  saveReceivedDataLog(uint8_t groupData[], uint8_t groupLength, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession);

    int  saveOwnDataLogInfo();
    int  savePart(char filePath[], uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, int* finishedBuilding, int flags);
    int  saveReceivedDataLogInfo(uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession);



// Variables
    PmmTelemetry*  mPmmTelemetry;
    PmmSd       *  mPmmSd;
    PmmSdSafeLog*  mPmmSdSafeLog;

    pmmSystemState mSystemMode;


// Self DataLog
    int      mIsLocked;
    uint8_t  mSystemSession;
    uint8_t  mDataLogId;

    uint8_t  mNumberVariables;
    char   * mVariableNameArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableTypeArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableSizeArray[MODULE_DATA_LOG_MAX_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAdrsArray[MODULE_DATA_LOG_MAX_VARIABLES]; // Adrs = Address!

    uint8_t  mGroupLength;

    uint8_t  mGroupTempData[PORT_DATA_LOG_MAX_PAYLOAD_LENGTH];  // Used in the saveOwnDataLog(). This, however, isn't used in the temeletry.


// Transmission
    uint8_t  mLogInfoContentArray[MODULE_LOG_INFO_CONTENT_MAX_LENGTH];
    uint16_t mLogInfoContentArrayLength;

    uint8_t  mDataLogInfoPackets;
    toBeSentPacketStructType mPacketStruct;

    // Update
    uint8_t  mUpdateModeReadyCounter, mUpdateModeDeployedCounter;
    uint8_t  mUpdateDataLogInfoCounter;

// Storage reception
    static constexpr const char* LOG_INFO_FILENAME = "DataLogInfo"; // https://stackoverflow.com/a/25323360/10247962
    static PmmSdAllocStatus mAllocStatusReceived       [PMM_TELEMETRY_ADDRESSES_FINAL_ALLOWED_SOURCE];
    static uint8_t          mAllocStatusReceivedSession[PMM_TELEMETRY_ADDRESSES_FINAL_ALLOWED_SOURCE];
    static char             mTempFilename [PMM_SD_FILENAME_MAX_LENGTH]; //
    static char             mTempFilename2[PMM_SD_FILENAME_MAX_LENGTH];
// Storage self
    PmmSdAllocStatus mAllocStatusSelfDataLog;
    char   mDataLogSelfDirPath[PMM_SD_FILENAME_MAX_LENGTH];


}; // End of the class

#endif
