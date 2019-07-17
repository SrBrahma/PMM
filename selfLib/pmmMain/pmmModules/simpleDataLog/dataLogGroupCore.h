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


// Auxiliar functions
uint8_t variableTypeToVariableSize(uint8_t variableType);
int     getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[] = NULL);


// In the improbable future.... There should be one class for each dataLog group in your system.
class PmmModuleDataLogGroupCore
{

public:
    PmmModuleDataLogGroupCore();
    int  init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession);

    int  addBasicInfo           (uint32_t* mainLoopCounterPtr, uint32_t* timeMillis); // Also adds the Transmission counter

    int  addImu(pmmImuStructType* pmmImuStructPtr);
    int  addGps(pmmGpsStructType* pmmGpsStruct   );

    // For a quick way to add a variable to the group. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like MODULE_DATA_LOG_TYPE_UINT8;
    int  addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);
    // -=-=-=-=-=-=-=-=


    // Transmission
    int  sendDataLog(uint8_t destinationAddress = PMM_TLM_ADDRESS_BROADCAST);


    // Storage
    int          saveOwnDataLog     ();


private:

    int  addMagnetometer        (void* magnetometerArray  );
    int  addGyroscope           (void* gyroscopeArray     );
    int  addAccelerometer       (void* accelerometerArray );
    int  addTemperatureMpu      (void* mpuTemperaturePtr  );
    int  addBarometer           (void* barometerPtr       );
    int  addAltitudeBarometer   (void* altitudePtr        );
    int  addTemperatureBmp      (void* barometerTempPtr   );


    int  includeVariable(const char*  variableName,   uint8_t variableType, void* variableAddress);
    int  includeArray   (const char** variablesNames, uint8_t arrayType,    void* arrayAddress, uint8_t arraySize);

    int  saveOwnDataLogInfo();

    uint8_t      getNumberOfVariables();
    const char** getVariableNameArray();
    uint8_t*     getVariableTypeArray();
    uint8_t*     getVariableSizeArray();
    uint8_t**    getVariableAdrsArray();
    uint8_t      getGroupLength();


    PmmTelemetry*  mPmmTelemetryPtr;
    PmmSd       *  mPmmSdPtr;

    uint8_t  mDataLogGroupId;
    uint8_t  mSystemSession;


    uint8_t  mNumberVariables;
    char   * mVariableNameArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableTypeArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableSizeArray[MODULE_DATA_LOG_MAX_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAdrsArray[MODULE_DATA_LOG_MAX_VARIABLES]; // Adrs = Address!
    uint8_t  mGroupLength;  // The sum of all sizes.

    uint32_t mTransmissionCounter;


    // These are important strings. The transmitter and the receiver must have in commom. The other variables strings not listed here can be freely changed.
    static constexpr const char* PMM_DATA_LOG_TRANSMISSION_COUNTER_STRING PROGMEM = "transmissionCounter"; // https://forum.arduino.cc/index.php?topic=420956.0
    static constexpr const char* PMM_DATA_LOG_MAIN_LOOP_COUNTER_STRING    PROGMEM = "mainLoopCounter";
    static constexpr const char* PMM_DATA_LOG_SYSTEM_TIME_MILLIS_STRING   PROGMEM = "systemTime(ms)";
    static constexpr const char* PMM_DATA_LOG_RAW_ALTITUDE_STRING         PROGMEM = "rawAltitudeBarometer(m)";
    static constexpr const char* PMM_DATA_LOG_ALTITUDE_STRING             PROGMEM = "AltitudeBarometer(m)";
    static constexpr const char* PMM_DATA_LOG_GPS_LATITUDE_STRING         PROGMEM = "gpsLatitude";
    static constexpr const char* PMM_DATA_LOG_GPS_LONGITUDE_STRING        PROGMEM = "gpsLongitude";



};

#endif