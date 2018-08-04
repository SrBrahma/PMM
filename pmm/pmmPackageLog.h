/* pmmPackageLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_PACKAGE_h
#define PMM_PACKAGE_h

#include <pmmConsts.h>
#include <pmmGps.h> // for GPS struct
#include <pmmImu.h> // for IMU struct

const char PMM_TELEMETRY_ALTITUDE_DEFAULT_STRING[] =    {"altitude(m)"};
const char PMM_TELEMETRY_GPS_LAT_DEFAULT_STRING[] =     {"gpsLongitude"};
const char PMM_TELEMETRY_GPS_LON_DEFAULT_STRING[] =     {"gpsLatitude"};

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

#define PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH 9 // ["MLIN"][CRC of the actual packet: 2B][CRC of the actual package: 2B][Packet X of Y - 1: 1B]
// Header for all packets:
// [0~3] Package Header
// [4~5] CRC of the packet
// [6~7] CRC of the package
// [8: 4 MSbits] Packet X [8: 4 LSbits] of a total of (Y - 1)

#define PMM_TELEMETRY_PACKAGE_LOG_INFO_MAX_PAYLOAD_LENGTH PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH
#define PMM_PACKAGE_LOG_INFO_RAW_MAX_LENGTH         1024
#define PMM_TELEMETRY_PACKAGE_LOG_INFO_MAX_PACKETS (PMM_PACKAGE_LOG_INFO_RAW_MAX_LENGTH + PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - 1) / (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PMM_TELEMETRY_PACKAGE_LOG_INFO_HEADER_LENGTH)
// Ceiling without ceil(). https://stackoverflow.com/a/2745086
// Wrote it for initializing the MLIN arrays in telemetry format.
// (I don't like the idea or doing uint8_t array[ceil(PMM_PACKAGE_LOG_INFO_RAW_MAX_LENGTH / PMM_TELEMETRY_PACKAGE_LOG_INFO_MAX_PAYLOAD_LENGTH)].
// I really don't know if it is a good idea to put a function inside a length declaration. ChangeMyMindMeme.jpg)
//
// packets = (packageRawSize + (headerSize * packets) + packetSize - 1) / packetSize
// with some math magic trick, it can be rewritten as
// packets = (packageRawSize + packetSize - 1) / (packetSize - headerSize)

class PmmPackageLog // Intended to have >1 Objects of this class! Maybe someday we will want to have one object for reception, and another for transmission!
{
private:

    uint8_t variableTypeToVariableSize(uint8_t variableType);
    void includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress);
    void includeArrayInPackage(const char **variableName, uint8_t arrayType, void *arrayAddress, uint8_t arraySize);

    // Build the Package Log Info Package
    void updatePackageLogInfoRaw();
    void updatePackageLogInfoInTelemetryFormat();

    // Uses the received packets via telemetry to get the Package Log Info
    void unitePackageInfoPackets();

    const char* mVariableNameArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t mVariableTypeArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t mVariableSizeArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAddressArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];

    uint16_t mLogInfoPackageCrc;
    uint8_t mLogNumberOfVariables;
    uint8_t mPackageLogSizeInBytes;

    uint8_t mPackageLogInfoRawArray[PMM_PACKAGE_LOG_INFO_RAW_MAX_LENGTH];
    uint16_t mPackageLogInfoRawArrayLength;

    uint8_t mPackageLogInfoNumberOfPackets;
    uint8_t mPackageLogInfoTelemetryArray[PMM_TELEMETRY_PACKAGE_LOG_INFO_MAX_PACKETS][PMM_TELEMETRY_MAX_PAYLOAD_LENGTH];
    uint8_t mPackageLogInfoTelemetryArrayLengths[PMM_TELEMETRY_PACKAGE_LOG_INFO_MAX_PACKETS];

public:

    PmmPackageLog();


    #if PMM_IS_PDA
        void receivedPackageInfo(uint8_t* packetArray, uint8_t packetSize);
    #endif

    // Add variables to the package log. The types are specified in pmmPackageLog.cpp.
    void addPackageBasicInfo(uint32_t* packageId, uint32_t* packageTimeMs);

    void addMagnetometer(void* magnetometerArray);
    void addGyroscope(void* gyroscopeArray);
    void addAccelerometer(void* accelerometerArray);
    void addBarometer(void* barometerPtr);
    void addAltitudeBarometer(void* altitudePtr);
    void addThermometer(void* thermometerPtr);

    void addImu(pmmImuStructType* pmmImuStructPtr);
    void addGps(pmmGpsStructType* pmmGpsStruct);

    // For a quick way to add a variable to the package.
    // Make sure the given variable name and the variable itself is static, global, "const PROGMEM", or any other way that the variable isn't lost during the program run.
    // Variable type follows the #define's like PMM_TELEMETRY_TYPE_UINT8;
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

}; // End of the class

#endif
