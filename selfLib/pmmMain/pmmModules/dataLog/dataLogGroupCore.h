/* dataLogBase.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_DATA_LOG_GROUP_h
#define PMM_MODULE_DATA_LOG_GROUP_h


#include <stdint.h>
#include "pmmTelemetry/telemetry.h"      // For transmitting
#include "pmmSd/sd.h"                    // For storing
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"
#include "pmmSd/consts.h"

#include "pmmModules/dataLog/dataLogInfo/logInfo.h"


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

#define MODULE_DATA_LOG_MAX_GROUPS                  3   // Be careful with this size.

// Auxiliar functions
uint8_t variableTypeToVariableSize(uint8_t variableType);
int     getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[] = NULL);
int     saveDataLog(PmmSdSafeLog* pmmSdSafeLog, uint8_t groupData[], char dirRelativePath[], PmmSdAllocStatus* allocStatus);


// In the improbable future.... There should be one class for each dataLog group in your system.
class PmmModuleDataLogGroupCore
{

public:
    PmmModuleDataLogGroupCore();
    int  init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, PmmSdSafeLog* pmmSdSafeLog, uint8_t systemSession, uint8_t dataLogGroupId);
    int  reset();

    // Add variables to the group.
    int  addTransmissionCounter ();
    int  addMainLoopCounter     (uint32_t* mainLoopCounterPtr);
    int  addTimeMillis          (uint32_t* timeMillisPtr);
    int  addBasicInfo           (uint32_t* mainLoopCounterPtr, uint32_t* timeMillis); // Adds the three above.

    int  addMagnetometer        (void* magnetometerArray  );
    int  addGyroscope           (void* gyroscopeArray     );
    int  addAccelerometer       (void* accelerometerArray );
    int  addTemperatureMpu      (void* mpuTemperaturePtr  );
    int  addBarometer           (void* barometerPtr       );
    int  addRawAltitudeBarometer(void* rawAltitudePressure); // Without filtering
    int  addAltitudeBarometer   (void* altitudePtr        );
    int  addTemperatureBmp      (void* barometerTempPtr   );

    int  addImu(pmmImuStructType* pmmImuStructPtr); // Adds all the sensors above.
    int  addGps(pmmGpsStructType* pmmGpsStruct   );

        // For a quick way to add a variable to the group. Make sure the given variable name and the variable itself is static, global,
        // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like MODULE_DATA_LOG_TYPE_UINT8;
    int  addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);
    // -=-=-=-=-=-=-=-=


    // Transmission
    int  sendDataLog(uint8_t destinationAddress = PMM_TLM_ADDRESS_BROADCAST, telemetryQueuePriorities priority = PMM_TLM_QUEUE_PRIORITY_LOW);
    int  sendDataLogInfo(uint8_t requestedPacket, uint8_t destinationAddress = PMM_TLM_ADDRESS_BROADCAST, telemetryQueuePriorities priority = PMM_TLM_QUEUE_PRIORITY_NORMAL);


    int          saveOwnDataLog     ();


    // LogInfo
    void         buildLogInfoArray(); // Updates the DataLogInfo
    uint8_t      getDataLogInfoPackets();


    // Getters
    int          getIsGroupLocked();

    uint8_t      getSystemSession();
    uint8_t      getDataLogGroupId();

    uint8_t      getNumberOfVariables();
    const char** getVariableNameArray();
    uint8_t*     getVariableTypeArray();
    uint8_t*     getVariableSizeArray();
    uint8_t**    getVariableAdrsArray();
    uint8_t      getGroupLength();



private:

    int  includeVariable(const char*  variableName,   uint8_t variableType, void* variableAddress);
    int  includeArray   (const char** variablesNames, uint8_t arrayType,    void* arrayAddress, uint8_t arraySize);

    int  saveOwnDataLogInfo(); // Automatically called at


    PmmTelemetry*  mPmmTelemetryPtr;
    PmmSd       *  mPmmSdPtr;
    PmmSdSafeLog*  mPmmSdSafeLogPtr;

    uint8_t  mDataLogGroupId;
    uint8_t  mSystemSession;


    int      mIsGroupLocked;

    uint8_t  mNumberVariables;
    char   * mVariableNameArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableTypeArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableSizeArray[MODULE_DATA_LOG_MAX_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAdrsArray[MODULE_DATA_LOG_MAX_VARIABLES]; // Adrs = Address!
    uint8_t  mGroupLength;  // The sum of all sizes.

    uint32_t mTransmissionCounter;

    // Storage self
    PmmSdAllocStatus mAllocStatusSelfDataLog;
    char             mDataLogSelfDirPath[PMM_SD_FILENAME_MAX_LENGTH];

    // These are important strings. The transmitter and the receiver must have in commom. The other variables strings not listed here can be freely changed.
    static constexpr const char* PMM_DATA_LOG_TRANSMISSION_COUNTER_STRING PROGMEM = "transmissionCounter"; // https://forum.arduino.cc/index.php?topic=420956.0
    static constexpr const char* PMM_DATA_LOG_MAIN_LOOP_COUNTER_STRING    PROGMEM = "mainLoopCounter";
    static constexpr const char* PMM_DATA_LOG_SYSTEM_TIME_MILLIS_STRING   PROGMEM = "systemTime(ms)";
    static constexpr const char* PMM_DATA_LOG_RAW_ALTITUDE_STRING         PROGMEM = "rawAltitudeBarometer(m)";
    static constexpr const char* PMM_DATA_LOG_ALTITUDE_STRING             PROGMEM = "AltitudeBarometer(m)";
    static constexpr const char* PMM_DATA_LOG_GPS_LATITUDE_STRING         PROGMEM = "gpsLatitude";
    static constexpr const char* PMM_DATA_LOG_GPS_LONGITUDE_STRING        PROGMEM = "gpsLongitude";

    // LogInfo
    uint8_t  mLogInfoContentArray[MODULE_LOG_INFO_CONTENT_MAX_LENGTH];
    uint16_t mLogInfoContentArrayLength;
    uint8_t  mDataLogInfoPackets;

};

#endif