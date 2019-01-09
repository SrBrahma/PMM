/* dataLogBase.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_DATA_LOG_GROUP_h
#define PMM_MODULE_DATA_LOG_GROUP_h

#include <stdint.h>
#include "pmmImu/pmmImu.h"
#include "pmmGps/pmmGps.h"


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

#define MODULE_DATA_LOG_MAX_GROUPS                  3 // Be careful with this size.

class PmmModuleDataLogGroup
{

public:
    int  newGroup(uint8_t systemSession, uint8_t* dataLogGroupId);
    int  getDataLogGroupCorePtr(uint8_t dataLogGroupId);
    int  removeGroup(uint8_t dataLogGroupId);

    class PmmModuleDataLogGroupCore
    {
    public:
        int  addTransmissionCounter(uint32_t* transmissionCounter);
        int  addMainLoopCounter    (uint32_t* mainLoopCounter);
        int  addTimeMillis         (uint32_t* timeMillis);

        int  addMagnetometer        (void* magnetometerArray );
        int  addGyroscope           (void* gyroscopeArray    );
        int  addAccelerometer       (void* accelerometerArray);
        int  addTemperatureMpu      (void* mpuTemperaturePtr );
        int  addBarometer           (void* barometerPtr      );
        int  addRawAltitudeBarometer(void* rawAltitudePressure); // Without filtering
        int  addAltitudeBarometer   (void* altitudePtr       );
        int  addTemperatureBmp      (void* barometerTempPtr  );

        int  addImu(pmmImuStructType* pmmImuStructPtr); // Adds all the sensors above.
        int  addGps(pmmGpsStructType* pmmGpsStruct   );

        // For a quick way to add a variable to the package. Make sure the given variable name and the variable itself is static, global,
        // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like MODULE_DATA_LOG_TYPE_UINT8;
        int  addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);

        // Getters
        int          lockGroup();

        int          getIsGroupLocked();

        uint8_t      getSystemSession();
        uint8_t      getDataLogGroupId();

        uint8_t      getNumberOfVariables();
        const char** getVariableNameArray();
        uint8_t*     getVariableTypeArray();
        uint8_t*     getVariableSizeArray();
        uint8_t**    getVariableAdrsArray();

    private:
        PmmModuleDataLogGroupCore();
        
        int      includeVariable(const char*  variableName,   uint8_t variableType, void* variableAddress);
        int      includeArray   (const char** variablesNames, uint8_t arrayType,    void* arrayAddress, uint8_t arraySize);

        int      mIsGroupLocked;
        uint8_t  mSystemSession;
        uint8_t  mDataLogGroupId;

        uint8_t  mNumberVariables;
        char   * mVariableNameArray[MODULE_DATA_LOG_MAX_VARIABLES];
        uint8_t  mVariableTypeArray[MODULE_DATA_LOG_MAX_VARIABLES];
        uint8_t  mVariableSizeArray[MODULE_DATA_LOG_MAX_VARIABLES]; // For a faster size access for the telemetry
        uint8_t* mVariableAdrsArray[MODULE_DATA_LOG_MAX_VARIABLES]; // Adrs = Address!

        uint8_t  mGroupLength;
    };

protected:
    // Auxiliar functions
    uint8_t      variableTypeToVariableSize(uint8_t variableType);



private:






};

#endif