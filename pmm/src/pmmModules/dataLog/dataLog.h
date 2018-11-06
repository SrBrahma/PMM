/* PmmModuleDataLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_PORT_LOG_h
#define PMM_PORT_LOG_h

#include "pmmTelemetry/pmmTelemetry.h"
#include "pmmSd/pmmSd.h"
#include "pmmGps/pmmGps.h" // for GPS struct
#include "pmmImu/pmmImu.h" // for IMU struct



// 000X 0-1 (1 byte)
#define PMM_EXTENSION_DATA_LOG_TYPE_UINT8    0
#define PMM_EXTENSION_DATA_LOG_TYPE_INT8     1

// 00X0 2-3 (2 bytes)
#define PMM_EXTENSION_DATA_LOG_TYPE_UINT16   2
#define PMM_EXTENSION_DATA_LOG_TYPE_INT16    3

// 0X00 4-8 (4 bytes)
#define PMM_EXTENSION_DATA_LOG_TYPE_UINT32   4
#define PMM_EXTENSION_DATA_LOG_TYPE_INT32    5
#define PMM_EXTENSION_DATA_LOG_TYPE_FLOAT    6

// X000 8-15 (8 bytes)
#define PMM_EXTENSION_DATA_LOG_TYPE_INT64    8
#define PMM_EXTENSION_DATA_LOG_TYPE_UINT64   9
#define PMM_EXTENSION_DATA_LOG_TYPE_DOUBLE   10



// DataLog and LogInfo Defines (Which I will as DATA_LOG anyway)

    #define PMM_EXTENSION_DATA_LOG_MAX_VARIABLES            50  // This must be the same for the transmitter and the receptor, for now.



// DataLog Defines

    #define PMM_EXTENSION_DATA_LOG_MAX_STRING_LENGTH        30 // To avoid really big faulty strings that would mess the system. Includes the '\0'.

    #define PMM_PORT_DATA_LOG_INDEX_CRC_8_HEADER            0
    #define PMM_PORT_DATA_LOG_INDEX_SESSION_ID              1
    #define PMM_PORT_DATA_LOG_INDEX_LOG_INFO_CRC_LSB        2
    #define PMM_PORT_DATA_LOG_INDEX_LOG_INFO_CRC_MSB        3
    #define PMM_PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB      4
    #define PMM_PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB      5
    // Total header length is equal to...
    #define PMM_PORT_DATA_LOG_HEADER_LENGTH                 6

    #define PMM_PORT_DATA_LOG_MAX_PAYLOAD_LENGTH            (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_PORT_DATA_LOG_HEADER_LENGTH)



// LogInfo Defines
    //    --------------------- 1.0 ------------------------
    //    [Positions] : [ Function ] : [ Length in Bytes ]

    //    a) [0,1] : [ CRC 16 of the current Packet ] : [ 2 ]
    //    b) [ 2 ] : [ Session Identifier ......... ] : [ 1 ]
    //    c) [ 3 ] : [ Packet X of (Y - 1) ........ ] : [ 1 ]
    //    d) [4,5] : [ CRC 16 of all Payloads Parts ] : [ 2 ]

    //                        Total header length = 6 bytes.
    //    --------------------------------------------------
    #define PMM_PORT_LOG_INFO_INDEX_CRC_PACKET_LSB          0
    #define PMM_PORT_LOG_INFO_INDEX_CRC_PACKET_MSB          1
    #define PMM_PORT_LOG_INFO_INDEX_SESSION_ID              2
    #define PMM_PORT_LOG_INFO_INDEX_PACKET_X_OF_Y_MINUS_1   3
    #define PMM_PORT_LOG_INFO_INDEX_CRC_PACKAGE_LSB         4
    #define PMM_PORT_LOG_INFO_INDEX_CRC_PACKAGE_MSB         5
    
    // Total header length is equal to...
    #define PMM_PORT_LOG_INFO_HEADER_LENGTH                 6

    #define PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH            (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_PORT_LOG_INFO_HEADER_LENGTH)

    #define PMM_PORT_LOG_INFO_RAW_PAYLOAD_MAX_LENGTH        2000 // A slightly random number. Thinking only on the strings, which occupies most of the
                                                                 //  length, 50 variables * 30 chars = 1500 bytes. 2kB for to be sure it won't overflow.
    
    #define PMM_PORT_LOG_INFO_MAX_PACKETS                   ((PMM_PORT_LOG_INFO_RAW_PAYLOAD_MAX_LENGTH + PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH - 1) / PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
    // Ceiling without ceil(). https://stackoverflow.com/a/2745086



class PmmModuleDataLog // Intended to have >1 Objects of this class, one for each DataLog Package
{

public:

    PmmModuleDataLog();

    int init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t* systemSessionPtr, uint32_t* packageId, uint32_t* packageTimeMsPtr);

    // Transmission
    void updateLogInfoRawPayload(); // Updates the LogInfo
    int sendDataLog();


    // Reception
    void receivedDataLog(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);
    void receivedLogInfo(uint8_t payload[], telemetryPacketInfoStructType* packetStatus);


    // Add variables to the package log. The types are specified in PmmModuleDataLog.cpp.
    void addMagnetometer     (void* magnetometerArray);
    void addGyroscope        (void* gyroscopeArray);
    void addAccelerometer    (void* accelerometerArray);
    void addBarometer        (void* barometerPtr);
    void addAltitudeBarometer(void* altitudePtr);
    void addThermometer      (void* thermometerPtr);

    void addImu(pmmImuStructType* pmmImuStructPtr); // Adds all the sensors above.
    void addGps(pmmGpsStructType* pmmGpsStruct);

    // For a quick way to add a variable to the package. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like PMM_EXTENSION_DATA_LOG_TYPE_UINT8;
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

    // Add variables to the package log. The types are specified in PmmModuleDataLog.cpp.
    void addPackageBasicInfo(uint32_t* packageId, uint32_t* packageTimeMs);


    uint8_t variableTypeToVariableSize(uint8_t variableType);
    void includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress);
    void includeArrayInPackage(const char **variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize);


    // Build the Package Log Info Package
    void updatePackageLogInfoRaw();
    void updatePackageLogInfoInTelemetryFormat();


    // Uses the received packets via telemetry to get the Package Log Info
    void unitePackageInfoPackets();




    PmmTelemetry* mPmmTelemetry;
    PmmSd* mPmmSd;

    // Default variables added in every DataLog package:
    uint8_t*  mSystemSessionPtr;


    char*    mVariableNameArray   [PMM_EXTENSION_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableTypeArray   [PMM_EXTENSION_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableSizeArray   [PMM_EXTENSION_DATA_LOG_MAX_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAddressArray[PMM_EXTENSION_DATA_LOG_MAX_VARIABLES];


    uint16_t mLogInfoPackageCrc;
    uint8_t  mLogNumberOfVariables;
    uint8_t  mPackageLogSizeInBytes;


    uint8_t  mPackageLogInfoRawArray[PMM_PORT_LOG_INFO_RAW_PAYLOAD_MAX_LENGTH];
    uint16_t mLogInfoRawPayloadArrayLength;


    uint8_t  mPackageLogInfoNumberOfPackets;
    uint8_t  mPackageLogInfoTelemetryArray[PMM_PORT_LOG_INFO_MAX_PACKETS][PMM_PORT_LOG_INFO_MAX_PAYLOAD_LENGTH];
    uint8_t  mPackageLogInfoTelemetryArrayLengths[PMM_PORT_LOG_INFO_MAX_PACKETS];



}; // End of the class

#endif
