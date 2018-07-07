#ifndef PMM_PACKAGE_h
#define PMM_PACKAGE_h

#include <pmmConsts.h>
#include <pmmGps.h> // for GPS struct
#include <pmmImu.h> // for IMU struct

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

const char PMM_TELEMETRY_ALTITUDE_DEFAULT_STRING[] = {"altitude(m)"};
const char PMM_TELEMETRY_GPS_LAT_DEFAULT_STRING[] = {"gpsLongitude"};
const char PMM_TELEMETRY_GPS_LON_DEFAULT_STRING[] = {"gpsLatitude"};

typedef struct
{
    char variableName[PMM_TELEMETRY_LOG_NUMBER_VARIABLES][PMM_VARIABLE_STRING_LENGTH];
    uint8_t variableType[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t *variableAddress[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
} pmmLogPackageType;

typedef struct
{
    char variableName[PMM_VARIABLE_STRING_LENGTH];
    uint8_t variableType; // For sending the Variables Info Package
    uint8_t variableSize; // For fast access for RF / Physical Storage
} pmmLogPackageSenderStructType; // Remember / Learn that structs are padded to have a size multiple of 4.
// https://stackoverflow.com/questions/119123/why-isnt-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member


class PmmPackageLog
{
private:
    unsigned mActualNumberVariables; // unsigned for quicker access.
    uint8_t variableTypeToVariableSize(uint8_t variableType);
    void includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress);

public:
    void addPackageBasicInfo(uint32_t* packageId, uint32_t* packageTimeMs);
    void addMagnetometer(float magnetometerArray[3]);
    void addGyroscope(float gyroscopeArray[3]);
    void addAccelerometer(float accelerometerArray[3]);
    void addBarometer(float* barometerPtr);
    void addAltitudeBarometer(float* altitudePtr);
    void addThermometer(float* thermometerPtr);
    void addImu(pmmImuStructType *pmmImuStructPtr);
    void addGps(PmmGps pmmGps);

    // For a quick way to add a variable to the package.
    // Make sure the given variable name and the variable itself is static, global, "const PROGMEM", or any other way that the variable isn't lost during the program run.
    // Variable type follows the #define's like PMM_TELEMETRY_TYPE_UINT8;
    void addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);

    unsigned returnNumberOfVariables();
    int returnNumberOfBytes();

    // Yeah, public members, for faster access. Maybe on future will add getters/setters.
    const char *mVariableName[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t mVariableType[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t mVariableSize[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    void *mVariableAddress[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];

    #if PMM_SERIAL_DEBUG
        void debugPrintLogHeader();
        void debugPrintLogContent();
    #endif
};

#endif
