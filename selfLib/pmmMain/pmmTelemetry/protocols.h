#ifndef PMM_TLM_PROTOCOLS_h
#define PMM_TLM_PROTOCOLS_h

#include <stdint.h>             // For uintx_t types.
#include "pmmConsts.h"          // For ADDRESS_THIS_SYSTEM define.


#define PMM_TLM_MAX_PACKET_TOTAL_LENGTH         255 // Max LoRa Packet Size!

#define PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH     (PMM_TLM_MAX_PACKET_TOTAL_LENGTH - PMM_NEO_PROTOCOL_HEADER_LENGTH)

#define PMM_TLM_MAX_PAYLOAD_LENGTH              PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH // Since this is currently the only protocol.



// Addresses

// This is defined at pmmConsts.h for a more centralized control of the important defines, so the user won't have to access different files
// for normal operation.

// #define PMM_TLM_ADDRESS_THIS_SYSTEM   PMM_TLM_ADDRESS_THIS_SYSTEM_CONST_H


    #define PMM_TLM_ADDRESSES_INITIAL_ALLOWED_SOURCE        0x00
    #define PMM_TLM_ADDRESSES_FINAL_ALLOWED_SOURCE          0xF7

    // Source only addresses
    #define PMM_TLM_ADDRESSES_INITIAL_FORBIDDEN_DESTINATION 0xF0
        // #define No source only reserved addresses for now!
    #define PMM_TLM_ADDRESSES_FINAL_FORBIDDEN_DESTINATION   0xF8
    // End of Soure only addresses


    // Can't be source or destination addresses. Note that it is INSIDE the two limits of the forbidden addresses!
    #define PMM_TLM_ADDRESS_SELF                            0xF8    // Have some specifics uses in the code.


    // Destination only addresses
    #define PMM_TLM_ADDRESSES_INITIAL_FORBIDDEN_SOURCE      0xF8

        #define PMM_TLM_ADDRESS_BROADCAST                   0xFF    // Destination only. This is the address that indicates a broadcast

    #define PMM_TLM_ADDRESSES_FINAL_FORBIDDEN_SOURCE        0xFF
    // End of Destination only addresses

// End of addresses




// Allowed Protocols
#define PMM_TLM_PROTOCOLS_ACCEPTS_NEO_PROTOCOL    1   // So you can enable/deactivate certain protocols of being received!



// Protocols Indexes definitions
#define PMM_TLM_PROTOCOLS_INDEX_PROTOCOL          0   // Where is the protocol identifier in the packet

    // PMM Neo Protocol
    #define PMM_NEO_PROTOCOL_INDEX_PACKET_LENGTH        1   // How many bytes do we have on the payload?
    #define PMM_NEO_PROTOCOL_INDEX_SOURCE               2   // Who sent this packet?
    #define PMM_NEO_PROTOCOL_INDEX_DESTINATION          3   // Who this packet wants to reach?
    #define PMM_NEO_PROTOCOL_INDEX_PORT                 4   // Who this packet wants to reach?
    #define PMM_NEO_PROTOCOL_INDEX_HEADER_CRC           5   // Least significant byte
    // Total length is
    #define PMM_NEO_PROTOCOL_HEADER_LENGTH              6

    // The define below is defined on pmmTelemetry.h, as there was happening a circular dependency!
    // #define PMM_NEO_PROTOCOL_MAX_PAYLOAD_LENGTH         PMM_TLM_MAX_PAYLOAD_LENGTH - PMM_NEO_PROTOCOL_HEADER_LENGTH

    #define PMM_NEO_PROTOCOL_ID                         1



typedef struct
{
    uint8_t* payload;
    int8_t   snr;                // in dBm
    int8_t   rssi;               // in dBm
    uint8_t  protocol;
    uint8_t  sourceAddress;
    uint8_t  destinationAddress;
    uint8_t  port;
    uint8_t  payloadLength;
} receivedPacketAllInfoStructType;

typedef struct
{
    uint8_t packet[PMM_TLM_MAX_PACKET_TOTAL_LENGTH];
    uint8_t packetLength;
    int8_t  snr;
    int8_t  rssi; //in dBm
} receivedPacketPhysicalLayerInfoStructType;

typedef enum
{
    PMM_TLM_QUEUE_PRIORITY_HIGH,
    PMM_TLM_QUEUE_PRIORITY_NORMAL,
    PMM_TLM_QUEUE_PRIORITY_LOW
} telemetryQueuePriorities;

class PacketToBeSent
{
public:
    uint8_t payload[PMM_TLM_MAX_PAYLOAD_LENGTH]; // Yes, public; so you can write into it directly.
    void    addInfo(uint8_t protocol, uint8_t sourceAddress, uint8_t destinationAddress, uint8_t port, uint8_t payloadLength, telemetryQueuePriorities priority = PMM_TLM_QUEUE_PRIORITY_NORMAL);
    uint8_t getProtocol();
    uint8_t getSourceAddress();
    uint8_t getDestinationAddress();
    uint8_t getPort();
    uint8_t getPayloadLength();
    telemetryQueuePriorities getPriority();
private:
    uint8_t mProtocol;
    uint8_t mSourceAddress;
    uint8_t mDestinationAddress;
    uint8_t mPort;
    uint8_t mPayloadLength;
    telemetryQueuePriorities mPriority;
};

// ===== Reception functions =====
// This function checks the received telemetry packet:
//
// 1) Check if the length of the packet is > 0.
// 2) Checks the protocol.
// 3) Checks the length of the packet again, now based on the protocol.
// 4) Checks the CRC, if handled by the protocol.
// 5) Check if the Destination Address is to this device and if is valid (there are forbidden addresses).
// 6) Check if the Source Address is valid (there are forbidden addresses).
//
// Ok return value is 0.
// If any of these checks fails, another value will be returned and the packet, discarded.
// These steps can be slightly different from protocol to protocol.
int  validateReceivedPacket(uint8_t packet[], uint8_t packetLength, uint8_t thisAddress, int promiscuousMode);

int  getReceivedPacketAllInfoStruct(receivedPacketPhysicalLayerInfoStructType* receivedPacketPhysicalLayerStruct, receivedPacketAllInfoStructType* receivedPacketAllInfoStruct);

// ===== Transmissin functions =====
int  buildPacket (uint8_t packet[], uint8_t* packetLength, PacketToBeSent* toBeSentTelemetryPacketInfoStruct);

uint8_t protocolHeaderLength(uint8_t protocol);

#endif