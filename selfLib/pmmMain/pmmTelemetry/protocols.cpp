#include <stdint.h>

#include "crc.h"
#include "byteSelection.h"

#include "pmmTelemetry/protocols.h"



// Returns the Header Length according to the given protocol. Returns 0 if invalid protocol.
uint8_t protocolHeaderLength(uint8_t protocol)
{
    switch (protocol)
    {
        case PMM_NEO_PROTOCOL_ID:
            return PMM_NEO_PROTOCOL_HEADER_LENGTH;
        default:
            return 0;
    }
}

// Adds the corresponding header depending on the protocol.
// It assumes you already checked the total length of the packet to be sent.
int  buildPacket(uint8_t destinationArray[], uint8_t* destinationPacketLength, PacketToBeSent* packetToBeSent)
{
    // 1) Test the given array
    if (!destinationArray)     return 1;
    if (!destinationPacketLength)         return 2;
    if (!packetToBeSent) return 3;

    // 2) Which protocol are we using?
    switch (packetToBeSent->getProtocol())
    {
        // NEO
        case PMM_NEO_PROTOCOL_ID:
        {
            // NEO, 3) Write the Protocol ID.
            destinationArray[PMM_TLM_PROTOCOLS_INDEX_PROTOCOL]     = PMM_NEO_PROTOCOL_ID;
            // NEO, 4) Write the Source Address
            destinationArray[PMM_NEO_PROTOCOL_INDEX_SOURCE]        = packetToBeSent->getSourceAddress();
            // NEO, 5) Write the Destination Address
            destinationArray[PMM_NEO_PROTOCOL_INDEX_DESTINATION]   = packetToBeSent->getDestinationAddress();
            // NEO, 6) Write the Port
            destinationArray[PMM_NEO_PROTOCOL_INDEX_PORT]          = packetToBeSent->getPort();
            // NEO, 7) Write the Payload Length.
            destinationArray[PMM_NEO_PROTOCOL_INDEX_PACKET_LENGTH] = packetToBeSent->getPayloadLength() + PMM_NEO_PROTOCOL_HEADER_LENGTH;
            // NEO, 8) Write the CRC of this header
            destinationArray[PMM_NEO_PROTOCOL_INDEX_HEADER_CRC]    = crc8(destinationArray, PMM_NEO_PROTOCOL_INDEX_HEADER_CRC); // The length is the same as the index of the CRC

            *destinationPacketLength = PMM_NEO_PROTOCOL_HEADER_LENGTH;

            memcpy(destinationArray + PMM_NEO_PROTOCOL_HEADER_LENGTH, packetToBeSent->payload, packetToBeSent->getPayloadLength());
            *destinationPacketLength += packetToBeSent->getPayloadLength();

            break;
        }

        default:
            return 4;   // Invalid given protocol
    }

    return 0;
}


// RECEPTION =======================================================================


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
//
// This is called by handleInterrupt() in RH_RF95.cpp so it takes less time for the system to acknowledge that the current packet is invalid,
// So we won't miss a good one that would appear just after this bad one. But, it's possible to the pmmTelemetry.cpp handle this, but I chose this way.
int validateReceivedPacket(uint8_t packet[], uint8_t packetLength, uint8_t thisAddress, int promiscuousMode)
{
    // 1) Check if the length of the packet is > 0.
    if (packetLength == 0) // We need a packet with a length of at least 1 to get its protocol.
        return -1;

    // 2) Checks the protocol.
    switch(packet[PMM_TLM_PROTOCOLS_INDEX_PROTOCOL])
    {
        #if PMM_TLM_PROTOCOLS_ACCEPTS_NEO_PROTOCOL
            case PMM_NEO_PROTOCOL_ID:
            {
                // NEO, 3.1) Checks the length of the packet again, now based on the protocol.
                if (packetLength < PMM_NEO_PROTOCOL_HEADER_LENGTH)
                    return 1; // Too short to be a real message

                // NEO, 3.2) Compares the length given by the Physical Layer (LoRa) and by the Transport Layer (Neo Protocol).
                if (packetLength != packet[PMM_NEO_PROTOCOL_INDEX_PACKET_LENGTH])
                    return 2;

                // NEO, 4) Checks the CRC.
                if (packet[PMM_NEO_PROTOCOL_INDEX_HEADER_CRC] != crc8(packet, PMM_NEO_PROTOCOL_HEADER_LENGTH))
                    return 3;

                // NEO, 5.1) Check if the Destination Address is to this device...
                if ((packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] != thisAddress) && !promiscuousMode)
                    return 4;

                // NEO, 5.2) ... and if is valid (there are forbidden addresses -- promiscuousMode isn't DUMB mode!)
                // Allowing forbidden addresses, like the PMM_TLM_ADDRESS_SELF, would mess the system / could be used for attacks.
                if (packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] >= PMM_TLM_ADDRESSES_INITIAL_FORBIDDEN_DESTINATION &&
                    packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] <= PMM_TLM_ADDRESSES_FINAL_FORBIDDEN_DESTINATION)
                    return 5;

                // NEO, 6) Check if the Source Address is valid (there are forbidden addresses).
                if (packet[PMM_NEO_PROTOCOL_INDEX_SOURCE] >= PMM_TLM_ADDRESSES_INITIAL_FORBIDDEN_SOURCE &&
                    packet[PMM_NEO_PROTOCOL_INDEX_SOURCE] <= PMM_TLM_ADDRESSES_FINAL_FORBIDDEN_SOURCE)
                    return 6;

                return 0;   // Successful.
            }
        #endif

        default:
            return -2;   // Invalid Protocol.
    }

}

// It assumes the packet was already validated by the validateReceivedPacket() function, which is called in handleInterrupt() function (in case of our rfm95w).
int getReceivedPacketAllInfoStruct(receivedPacketPhysicalLayerInfoStructType* receivedPacketPhysicalLayerStruct, receivedPacketAllInfoStructType* receivedPacketAllInfoStruct)
{
    if (!receivedPacketPhysicalLayerStruct) return 1;
    if (!receivedPacketAllInfoStruct)       return 2;

    // 1) Which protocol is this packet using?
    switch(receivedPacketPhysicalLayerStruct->packet[PMM_TLM_PROTOCOLS_INDEX_PROTOCOL])
    {
        case PMM_NEO_PROTOCOL_ID:
            receivedPacketAllInfoStruct->payload            = receivedPacketPhysicalLayerStruct->packet + PMM_NEO_PROTOCOL_HEADER_LENGTH;
            receivedPacketAllInfoStruct->snr                = receivedPacketPhysicalLayerStruct->snr;
            receivedPacketAllInfoStruct->rssi               = receivedPacketPhysicalLayerStruct->rssi;
            receivedPacketAllInfoStruct->protocol           = PMM_NEO_PROTOCOL_ID;
            receivedPacketAllInfoStruct->sourceAddress      = receivedPacketPhysicalLayerStruct->packet[PMM_NEO_PROTOCOL_INDEX_SOURCE];
            receivedPacketAllInfoStruct->destinationAddress = receivedPacketPhysicalLayerStruct->packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION];
            receivedPacketAllInfoStruct->port               = receivedPacketPhysicalLayerStruct->packet[PMM_NEO_PROTOCOL_INDEX_PORT];
            receivedPacketAllInfoStruct->payloadLength      = receivedPacketPhysicalLayerStruct->packet[PMM_NEO_PROTOCOL_INDEX_PACKET_LENGTH] - PMM_NEO_PROTOCOL_HEADER_LENGTH;

            break;
    }

    return 0;
}

void  PacketToBeSent::addInfo(uint8_t protocol, uint8_t sourceAddress, uint8_t destinationAddress, uint8_t port, uint8_t payloadLength, telemetryQueuePriorities priority)
{
    mProtocol           = protocol;
    mSourceAddress      = sourceAddress;
    mDestinationAddress = destinationAddress;
    mPort               = port;
    mPayloadLength      = payloadLength;
    mPriority           = priority;
}

uint8_t  PacketToBeSent::getProtocol()                 { return mProtocol;      }
uint8_t  PacketToBeSent::getSourceAddress()            { return mSourceAddress; }
uint8_t  PacketToBeSent::getDestinationAddress()       { return mDestinationAddress;}
uint8_t  PacketToBeSent::getPort()                     { return mPort;          }
uint8_t  PacketToBeSent::getPayloadLength()            { return mPayloadLength; }
telemetryQueuePriorities PacketToBeSent::getPriority() { return mPriority;      }

