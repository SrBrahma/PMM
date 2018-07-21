/* pmmPackageLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

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



class PmmPackageLog
{
private:

    uint8_t variableTypeToVariableSize(uint8_t variableType);
    void includeVariableInPackage(const char *variableName, uint8_t variableType, void *variableAddress);

    const char* mVariableNameArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t mVariableTypeArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t mVariableSizeArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];
    uint8_t* mVariableAddressArray[PMM_TELEMETRY_LOG_NUMBER_VARIABLES];

    uint8_t mActualNumberVariables;
    uint8_t mPackageSizeInBytes;

public:
    PmmPackageLog();
    void addPackageBasicInfo(uint32_t* packageId, uint32_t* packageTimeMs);

    #define PMM_PACKAGE_LOG_MAGNETOMETER_TYPE float
    void addMagnetometer(PMM_PACKAGE_LOG_MAGNETOMETER_TYPE magnetometerArray[3]);

    #define PMM_PACKAGE_LOG_GYROSCOPE_TYPE float
    void addGyroscope(PMM_PACKAGE_LOG_GYROSCOPE_TYPE gyroscopeArray[3]);

    #define PMM_PACKAGE_LOG_ACCELEROMETER_TYPE float
    void addAccelerometer(PMM_PACKAGE_LOG_ACCELEROMETER_TYPE accelerometerArray[3]);

    #define PMM_PACKAGE_LOG_BAROMETER_TYPE float
    void addBarometer(PMM_PACKAGE_LOG_BAROMETER_TYPE* barometerPtr);

    #define PMM_PACKAGE_LOG_ALTITUDE_BAROMETER_TYPE float
    void addAltitudeBarometer(PMM_PACKAGE_LOG_ALTITUDE_BAROMETER_TYPE* altitudePtr);

    #define PMM_PACKAGE_THERMOMETER_TYPE float
    void addThermometer(PMM_PACKAGE_THERMOMETER_TYPE* thermometerPtr);

    void addImu(pmmImuStructType* pmmImuStructPtr);
    void addGps(pmmGpsStructType* pmmGpsStruct);

    // For a quick way to add a variable to the package.
    // Make sure the given variable name and the variable itself is static, global, "const PROGMEM", or any other way that the variable isn't lost during the program run.
    // Variable type follows the #define's like PMM_TELEMETRY_TYPE_UINT8;
    void addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);

    uint8_t getNumberOfVariables();
    uint8_t getPackageSizeInBytes();

    const char** getVariableNameArray();
    uint8_t* getVariableTypeArray();
    uint8_t* getVariableSizeArray();
    uint8_t** getVariableAddressArray();

    #if PMM_DEBUG_SERIAL
        void debugPrintLogHeader();
        void debugPrintLogContent();
    #endif


};

#endif
