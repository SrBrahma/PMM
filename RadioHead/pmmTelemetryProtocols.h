#ifndef PMM_TELEMETRY_PROTOCOLS_h
#define PMM_TELEMETRY_PROTOCOLS_h

#define PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL 0    // Where is the protocol identifier in the packet

#define PMM_NEO_PROTOCOL_INDEX_SOURCE       1   // Who sent this packet?
#define PMM_NEO_PROTOCOL_INDEX_DESTINATION  2   // Who this packet wants to reach?
#define PMM_NEO_PROTOCOL_INDEX_CRC_LSB      3   // Least significant byte
#define PMM_NEO_PROTOCOL_INDEX_CRC_MSB      4   // Most significant byte
#define PMM_NEO_PROTOCOL_INDEX_PORT         5   // What this packet is transferring?

#define PMM_TELEMETRY_MINIMUM_PACKET_TOTAL_LENGTH 6 // The tota
typedef enum
{
    PMM_NEO_PROTOCOL = 0
} pmmTelemetryProtocolsType;



#endif
