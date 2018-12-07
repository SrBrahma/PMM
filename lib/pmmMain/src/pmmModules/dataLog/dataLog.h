/* PmmModuleDataLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PORT_LOG_h
#define PORT_LOG_h

#include "pmmTelemetry/pmmTelemetry.h"
#include "pmmSd/pmmSd.h"
#include "pmmGps/pmmGps.h" // for GPS struct
#include "pmmImu/pmmImu.h" // for IMU struct



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

    #define MODULE_DATA_LOG_MAX_VARIABLES              50  // This must be the same value for the transmitter and the receptor.
    
    #define MODULE_DATA_LOG_MAX_STRING_LENGTH          30  // The maximum Variable String. Includes the '\0'.


// DataLog Defines

    #define PORT_DATA_LOG_INDEX_CRC_8_HEADER            0
    #define PORT_DATA_LOG_INDEX_SESSION_ID              1
    #define PORT_DATA_LOG_INDEX_DATA_LOG_INFO_ID        2
    #define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_LSB      3
    #define PORT_DATA_LOG_INDEX_CRC_16_PAYLOAD_MSB      4
    // Total header length is equal to...
    #define PORT_DATA_LOG_HEADER_LENGTH                 5

    #define PORT_DATA_LOG_MAX_PAYLOAD_LENGTH            (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PORT_DATA_LOG_HEADER_LENGTH)




    #define PORT_LOG_INFO_INDEX_CRC_LSB                 0
    #define PORT_LOG_INFO_INDEX_CRC_MSB                 1
    #define PORT_LOG_INFO_INDEX_SESSION_ID              2
    #define PORT_LOG_INFO_INDEX_PACKET_X                3
    #define PORT_LOG_INFO_INDEX_OF_Y_PACKETS            4
    #define PORT_LOG_INFO_INDEX_LOG_INFO_ID             5
    
    // Total header length is equal to...
    #define PORT_LOG_INFO_HEADER_LENGTH                 6

    // The maximum payload length per packet.
    #define PORT_LOG_INFO_MAX_PAYLOAD_LENGTH            (PMM_TELEMETRY_MAX_PAYLOAD_LENGTH - PORT_LOG_INFO_HEADER_LENGTH)

    // When sending the types of the variables (4 bits each type), they are grouped into 1 byte, to make the telemetry packet smaller (read the Telemetry Guide).
    //   If the number of variables is odd, the last variable type won't be grouped with another variable type, as there isn't another one,
    //   but it will still take 1 byte on the telemetry packet to send it. So, it's the same as: maxLengthVariablesType = ceil(numberVariables/2).
    #define PORT_LOG_INFO_VARIABLE_TYPES_MAX_LENGTH     ((MODULE_DATA_LOG_MAX_VARIABLES + 2 - 1) / 2)
                                                            // Ceiling without ceil(). https://stackoverflow.com/a/2745086

    // The total payload length, combining all the packets.
    #define PORT_LOG_INFO_COMBINED_PAYLOAD_MAX_LENGTH   (1 + PORT_LOG_INFO_VARIABLE_TYPES_MAX_LENGTH + MODULE_DATA_LOG_MAX_VARIABLES*MODULE_DATA_LOG_MAX_STRING_LENGTH)
   
    // How many packets are needed to send the Combined Payload.
    #define PORT_LOG_INFO_MAX_PACKETS                   ((PORT_LOG_INFO_COMBINED_PAYLOAD_MAX_LENGTH + PORT_LOG_INFO_MAX_PAYLOAD_LENGTH - 1) / PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
                                                            // Ceiling without ceil(). https://stackoverflow.com/a/2745086



class PmmModuleDataLog // Intended to have >1 Objects of this class, one for each DataLog Package
{

public:

    PmmModuleDataLog();

    int init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession, uint8_t dataLogInfoId, uint32_t* packageId, uint32_t* packageTimeMsPtr);

    // Transmission
    void updateLogInfoCombinedPayload(); // Updates the DataLogInfo
    int  sendDataLog();


    // Reception
    int  receivedDataLog(receivedPacketAllInfoStructType* packetInfo);
    int  receivedLogInfo(receivedPacketAllInfoStructType* packetInfo);


    // Add variables to the package log. Their types are specified in PmmModuleDataLog.cpp.
    void addMagnetometer     (void* magnetometerArray );
    void addGyroscope        (void* gyroscopeArray    );
    void addAccelerometer    (void* accelerometerArray);
    void addBarometer        (void* barometerPtr      );
    void addAltitudeBarometer(void* altitudePtr       );
    void addThermometer      (void* thermometerPtr    );

    void addImu(pmmImuStructType* pmmImuStructPtr); // Adds all the sensors above.
    void addGps(pmmGpsStructType* pmmGpsStruct   );

    // For a quick way to add a variable to the package. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like MODULE_DATA_LOG_TYPE_UINT8;
    void addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);


    // Getters
    uint8_t getNumberOfVariables     ();
    uint8_t getPackageLogSizeInBytes ();

    const char** getVariableNameArray();
    uint8_t*     getVariableTypeArray();
    uint8_t*     getVariableSizeArray();
    uint8_t**    getVariableAdrsArray();


    // Debug!
    #if PMM_DEBUG
        void debugPrintLogHeader ();
        void debugPrintLogContent();
    #endif



private:

    uint8_t variableTypeToVariableSize(uint8_t variableType);

    // Add variables to the package log. The types are specified in PmmModuleDataLog.cpp.
    void    addPackageBasicInfo(uint32_t* packageId, uint32_t* packageTimeMs);

    void includeVariableInPackage(const char*  variableName,   uint8_t variableType, void* variableAddress);
    void includeArrayInPackage   (const char** variablesNames, uint8_t arrayType,    void* arrayAddress, uint8_t arraySize);

    // Build the Package Log Info Package
    void updatePackageLogInfoRaw();
    int  updateLogInfoInTelemetryFormat(uint8_t requestedPacket, uint8_t arrayToCopyTo[], uint8_t* packetLength);

// Reception Functions

    // Uses the received packets via telemetry to get the Package Log Info
    void    unitePackageInfoPackets();



    PmmTelemetry* mPmmTelemetry;
    PmmSd       * mPmmSd;

// Self
    uint8_t  mSystemSession;
    uint8_t  mDataLogInfoId;


    // Transmission
    char   * mVariableNameArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableTypeArray[MODULE_DATA_LOG_MAX_VARIABLES];
    uint8_t  mVariableSizeArray[MODULE_DATA_LOG_MAX_VARIABLES]; // For a faster size access for the telemetry
    uint8_t* mVariableAdrsArray[MODULE_DATA_LOG_MAX_VARIABLES]; // Adrs = Address!


    uint8_t  mLogNumberOfVariables;
    uint8_t  mDataLogSize;


    uint8_t  mDataLogInfoTelemetryRawArray[PORT_LOG_INFO_COMBINED_PAYLOAD_MAX_LENGTH];
    uint16_t mLogInfoRawPayloadArrayLength;


    uint8_t  mDataLogInfoPackets;

}; // End of the class

#endif
