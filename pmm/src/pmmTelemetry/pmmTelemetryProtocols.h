#ifndef PMM_TELEMETRY_PROTOCOLS_h
#define PMM_TELEMETRY_PROTOCOLS_h

#include <stdint.h>

// Addresses
#define RH_THIS_SYSTEM_ADDRESS  0x01    // You may change it!

    // Reserved addresses
    #define RH_INITIAL_RESERVED_ADDRESS 0xF0

        // Source only addresses
            #define RH_INITIAL_FORBIDDEN_DESTINATION_ADDRESS 0xF0
                // #define No source only reserved address for now!
            #define RH_FINAL_FORBIDDEN_DESTINATION_ADDRESS   0xF8
        // End of Soure only addresses


        // Can't be source or destination addresses. Note that it is between the two limits of the forbidden addresses!
            #define RH_SELF_ADDRESS         0xF8    // Have some specifics uses in the code.


        // Destination only addresses
            #define RH_INITIAL_FORBIDDEN_SOURCE_ADDRESS 0xF8

            #define RH_TO_ALL_PDA_ADDRESS   0xFE    // Destination only.
            #define RH_BROADCAST_ADDRESS    0xFF    // Destination only. This is the address that indicates a broadcast

            #define RH_FINAL_FORBIDDEN_SOURCE_ADDRESS   0xFF
        // End of Destination only addresses

    #define RH_FINAL_RESERVED_ADDRESS   0xFF
    // End of Reserved addresses
    
// End of addresses




// Allowed Protocols
#define PMM_TELEMETRY_PROTOCOLS_ACCEPTS_NEO_PROTOCOL    1   // So you can enable/deactivate certain protocols of being received!



// Protocols Indexes definitions
#define PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL          0   // Where is the protocol identifier in the packet

    // PMM Neo Protocol
    #define PMM_NEO_PROTOCOL_INDEX_SOURCE               1   // Who sent this packet?
    #define PMM_NEO_PROTOCOL_INDEX_DESTINATION          2   // Who this packet wants to reach?
    #define PMM_NEO_PROTOCOL_INDEX_PORT                 3   // Who this packet wants to reach?
    #define PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_LSB       4   // Least significant byte
    #define PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_MSB       5   // Most significant byte
    // Total length is
    #define PMM_NEO_PROTOCOL_HEADER_LENGTH              6   // The minimum length, counting



// Define instead of typedef enum, because I didn't wanted to use 4 bytes in telemetryProtocolsContentStructType just to store the few protocols
#define PMM_NEO_PROTOCOL_ID                 1



typedef struct
{
    uint8_t sourceAddress;
    uint8_t destinationAddress;
    uint8_t port;
    uint8_t payloadLength;
    bool    invalidAutoCrc;
    int8_t  snr;
    int16_t rssi; //in dBm
} telemetryPacketInfoStructType;

typedef struct
{
    uint8_t protocol;
    uint8_t sourceAddress;
    uint8_t destinationAddress;
    uint8_t port;
} telemetryProtocolsContentStructType;

#endif



uint8_t getProtocolHeaderLength(uint8_t protocol);



// Check the packet protocol and return the length of the header. If 0 is returned, the packet is invalid.
// It also checks if the Destination Address of the received packet is the same as the address of this system.
uint8_t validateReceivedPacketAndReturnProtocolHeaderLength(uint8_t packetData[], uint8_t bufferLength, uint8_t thisAddress, int promiscuousMode);










//
