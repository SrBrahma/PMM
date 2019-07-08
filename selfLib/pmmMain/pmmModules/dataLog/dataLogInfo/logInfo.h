#ifndef PMM_MODULE_DATA_LOG_INFO_h
#define PMM_MODULE_DATA_LOG_INFO_h

#include "pmmModules/dataLog/dataLogGroupCore.h"


#define PORT_LOG_INFO_INDEX_CRC_LSB                 0
#define PORT_LOG_INFO_INDEX_CRC_MSB                 1
#define PORT_LOG_INFO_INDEX_SESSION_ID              2
#define PORT_LOG_INFO_INDEX_CURRENT_PACKET          3
#define PORT_LOG_INFO_INDEX_TOTAL_PACKETS           4
#define PORT_LOG_INFO_INDEX_DATA_LOG_ID             5
#define PORT_LOG_INFO_INDEX_DATA_LOG_GROUP_LENGTH   6

// Total header length is equal to...
#define PORT_LOG_INFO_HEADER_LENGTH                 7
#define PORT_LOG_INFO_INDEX_PAYLOAD_START           PORT_LOG_INFO_HEADER_LENGTH

// The maximum payload length per packet.
#define PORT_LOG_INFO_MAX_PAYLOAD_LENGTH            (PMM_TLM_MAX_PAYLOAD_LENGTH - PORT_LOG_INFO_HEADER_LENGTH)

// When sending the types of the variables (4 bits each type), they are grouped into 1 byte, to make the telemetry packet smaller (read the Telemetry Guide).
//   If the number of variables is odd, the last variable type won't be grouped with another variable type, as there isn't another one,
//   but it will still take 1 byte on the telemetry packet to send it. So, it's the same as: maxLengthVariablesType = ceil(numberVariables/2).
#define MODULE_LOG_INFO_VARS_TYPES_MAX_LENGTH       ((MODULE_DATA_LOG_MAX_VARIABLES + 2 - 1) / 2)
                                                    // Ceiling without ceil(). https://stackoverflow.com/a/2745086

// The total DataLogInfo content length
#define MODULE_LOG_INFO_CONTENT_MAX_LENGTH          (1 + MODULE_LOG_INFO_VARS_TYPES_MAX_LENGTH + MODULE_DATA_LOG_MAX_VARIABLES * MODULE_DATA_LOG_MAX_STRING_LENGTH)

// How many packets are needed to send the Combined Payload.
#define PORT_LOG_INFO_MAX_PACKETS                   ((MODULE_LOG_INFO_CONTENT_MAX_LENGTH + PORT_LOG_INFO_MAX_PAYLOAD_LENGTH - 1) / PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
                                                    // Ceiling without ceil(). https://stackoverflow.com/a/2745086


#define LOG_INFO_FILENAME                           "DataLogInfo"


#endif