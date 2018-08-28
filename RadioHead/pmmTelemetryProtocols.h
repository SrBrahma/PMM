#ifndef PMM_TELEMETRY_PROTOCOLS_h
#define PMM_TELEMETRY_PROTOCOLS_h

#include <stdint.h>

#define PMM_TELEMETRY_PROTOCOLS_ACCEPTS_NEO_PROTOCOL    1       // So you can enable/deactivate certain protocols of being received!

#define PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL 0    // Where is the protocol identifier in the packet

#define PMM_NEO_PROTOCOL_INDEX_SOURCE               1   // Who sent this packet?
#define PMM_NEO_PROTOCOL_INDEX_DESTINATION          2   // Who this packet wants to reach?
#define PMM_NEO_PROTOCOL_INDEX_PORT                 3   // Who this packet wants to reach?
#define PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_LSB       4   // Least significant byte
#define PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_MSB       5   // Most significant byte

#define PMM_NEO_PROTOCOL_HEADER_LENGTH      6   // The minimum length, counting



typedef enum
{
    PMM_NEO_PROTOCOL_ID = 1
} pmmTelemetryProtocolsType;

typedef struct
{
    uint8_t sourceAddress;
    uint8_t destinationAddress;
    uint8_t port;
    uint8_t payloadLength;
     int8_t snr;
    int16_t rssi; //in dBm
} pmmTelemetryPacketStatusStructType;

#endif
