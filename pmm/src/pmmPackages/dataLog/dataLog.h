/* PmmPackageDataLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_PORT_LOG_h
#define PMM_PORT_LOG_h

#include "pmmConsts.h"
#include "pmmGps/pmmGps.h" // for GPS struct
#include "pmmImu/pmmImu.h" // for IMU struct
#include "pmmTelemetry/pmmTelemetry.h"



// 000X 0-1 (1 byte)
#define PMM_TELEMETRY_TYPE_UINT8    0
#define PMM_TELEMETRY_TYPE_INT8     1

// 00X0 2-3 (2 bytes)
#define PMM_TELEMETRY_TYPE_UINT16   2
#define PMM_TELEMETRY_TYPE_INT16    3

// 0X00 4-8 (4 bytes)
#define PMM_TELEMETRY_TYPE_UINT32   4
#define PMM_TELEMETRY_TYPE_INT32    5
#define PMM_TELEMETRY_TYPE_FLOAT    6

// X000 8-15 (8 bytes)
#define PMM_TELEMETRY_TYPE_INT64    8
#define PMM_TELEMETRY_TYPE_UINT64   9
#define PMM_TELEMETRY_TYPE_DOUBLE   10

// DataLog and LogInfo Defines (Which I will as DATA_LOG anyway)



// DataLog Defines

    #define PMM_PACKAGE_DATA_LOG_MAX_STRING_LENGTH          30 // To avoid really big faulty strings that would mess the system

    #define PMM_PORT_DATA_LOG_INDEX_CRC_HEADER              0
    #define PMM_PORT_DATA_LOG_INDEX_SESSION_ID              1
    #define PMM_PORT_DATA_LOG_INDEX_LOG_INFO_RELATED_CRC    2 // Is 2 bytes wide, CRC 16
    #define PMM_PORT_DATA_LOG_INDEX_LOG_LENGTH              4
    #define PMM_PORT_DATA_LOG_INDEX_CRC_OF_PAYLOAD          5 // Is 2 bytes wide, CRC 16

    #define PMM_PORT_DATA_LOG_HEADER_LENGTH                 7

    #define PMM_PORT_DATA_LOG_MAX_PAYLOAD_LENGTH            PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH - PMM_PORT_DATA_LOG_HEADER_LENGTH

// LogInfo Defines

    #define PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKET          0
    #define PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKET          1
    #define PMM_PORT_LOG_INFO_INDEX_LSB_CRC_PACKAGE         2
    #define PMM_PORT_LOG_INFO_INDEX_MSB_CRC_PACKAGE         3
    #define PMM_PORT_LOG_INFO_INDEX_PACKET_X_OF_Y_MINUS_1   4
    // =
    #define PMM_PORT_LOG_INFO_HEADER_LENGTH                 5

    #define PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH            PMM_PORT_DATA_LOG_HEADER_LENGTH
    // Header for all packets:
    // [0~1] CRC of the packet
    // [2~3] CRC of the package
    // [4: 4 MSbits] Packet X [8: 4 LSbits] of a total of (Y - 1)

    #define PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH  PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_PORT_LOG_INFO_HEADER_LENGTH
    #define PMM_PORT_LOG_INFO_RAW_MAX_LENGTH      3000 // A slightly random number. Thinking only on the strings, which occupies most of the length, 50 variables * 30 chars = 1500 bytes. 3kB for 
    #define PMM_PORT_LOG_INFO_MAX_PACKETS         (PMM_PORT_LOG_INFO_RAW_MAX_LENGTH + PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - 1) / (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_PORT_LOG_INFO_HEADER_LENGTH)
    // Ceiling without ceil(). https://stackoverflow.com/a/2745086
    // Wrote it for initializing the LogInfo arrays in telemetry format.

    // packets = (packageRawSize + (headerSize * packets) + packetSize - 1) / packetSize
    // with some math magic trick, it can be rewritten as
    // packets = (packageRawSize + packetSize - 1) / (packetSize - headerSize)





class PmmPackageDataLog // Intended to have >1 Objects of this class, on the future! Maybe someday we will want to have one object for reception, and another for transmission!
{

public:

    PmmPackageDataLog();

    int init(PmmTelemetry* pmmTelemetry, uint8_t* systemSessionPtr, uint8_t* miniSessionIdPtr, uint32_t* packageId, uint32_t* packageTimeMsPtr);

    // Transmission
    void updateLogInfoRawPayload(); // Updates the LogInfo


    // Reception
    void receivedDataLog(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);
    void receivedLogInfo(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);


    // Add variables to the package log. The types are specified in PmmPackageDataLog.cpp.
    void addMagnetometer(void* magnetometerArray);
    void addGyroscope(void* gyroscopeArray);
    void addAccelerometer(void* accelerometerArray);
    void addBarometer(void* barometerPtr);
    void addAltitudeBarometer(void* altitudePtr);
    void addThermometer(void* thermometerPtr);

    void addImu(pmmImuStructType* pmmImuStructPtr);
    void addGps(pmmGpsStructType* pmmGpsStruct);

    // For a quick way to add a variable to the package. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like PMM_TELEMETRY_TYPE_UINT8;
    void addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);


    // Getters
    uint8_t getNumberOfVariables();
    uint8_t getPackageLogSizeInBytes();

    const char** getVariableNameArray();
    uint8_t* getVariableTypeArray();
    uint8_t* getVariableSizeArray();
    uint8_t** getVariableAddressArray();


    // Debug!
    #if PMM_DEBUG_SERIAL
        void debugPrintLogHeader();
        void debugPrintLogContent();
    #endif



private:

    // Add variables to the package log. The types are specified in PmmPackageDataLog.cpp.
    void addPackageBasicInfo(uint8_t* miniSessionIdPtr, uint32_t* packageId, uint32_t* packageTimeMs);


    uint8_t variableTypeToVariableSize(uint8_t variableType);
    void includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress);
    void includeArrayInPackage(const char **variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize);


    // Build the Package Log Info Package
    void updatePackageLogInfoRaw();
    void updatePackageLogInfoInTelemetryFormat();


    // Uses the received packets via telemetry to get the Package Log Info
    void unitePackageInfoPackets();




    PmmTelemetry* mPmmTelemetry;


    // Default variables added in every DataLog package:
    uint8_t*  mSystemSessionPtr;


    char*    mVariableNameArray   [PMM_PORT_LOG_NUMBER_VARIABLES];
    uint8_t  mVariableTypeArray   [PMM_PORT_LOG_NUMBER_VARIABLES];
    uint8_t  mVariableSizeArray   [PMM_PORT_LOG_NUMBER_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAddressArray[PMM_PORT_LOG_NUMBER_VARIABLES];


    uint16_t mLogInfoPackageCrc;
    uint8_t  mLogNumberOfVariables;
    uint8_t  mPackageLogSizeInBytes;


    uint8_t  mPackageLogInfoRawArray[PMM_PORT_LOG_INFO_RAW_MAX_LENGTH];
    uint16_t mLogInfoRawPayloadArrayLength;


    uint8_t  mPackageLogInfoNumberOfPackets;
    uint8_t  mPackageLogInfoTelemetryArray[PMM_PORT_LOG_INFO_MAX_PACKETS][PMM_TELEMETRY_MAX_PAYLOAD_LENGTH];
    uint8_t  mPackageLogInfoTelemetryArrayLengths[PMM_PORT_LOG_INFO_MAX_PACKETS];



}; // End of the class

#endif
