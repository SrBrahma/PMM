#include <stdint.h>

#include "crc.h"
#include "byteSelection.h"

#include "pmmTelemetry/pmmTelemetryProtocols.h"



// It assumes the packet was already validated by the validateReceivedPacket() function, which is called in handleInterrupt() function (in case of our rfm95w).
void getReceivedPacketAllInfoStruct(uint8_t packet[], receivedPacketPhysicalLayerInfoStructType* receivedPacketPhysicalLayerInfoStruct, receivedPacketAllInfoStructType* receivedPacketAllInfoStruct)
{
    // 1) Which protocol is this packet using?
    switch(packet[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        case PMM_NEO_PROTOCOL_ID:
            receivedPacketAllInfoStruct->payload            = packet + PMM_NEO_PROTOCOL_HEADER_LENGTH;
            receivedPacketAllInfoStruct->snr                = receivedPacketPhysicalLayerInfoStruct->snr;
            receivedPacketAllInfoStruct->rssi               = receivedPacketPhysicalLayerInfoStruct->rssi;
            receivedPacketAllInfoStruct->protocol           = PMM_NEO_PROTOCOL_ID;
            receivedPacketAllInfoStruct->sourceAddress      = packet[PMM_NEO_PROTOCOL_INDEX_SOURCE];
            receivedPacketAllInfoStruct->destinationAddress = packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION];
            receivedPacketAllInfoStruct->port               = packet[PMM_NEO_PROTOCOL_INDEX_PORT];
            receivedPacketAllInfoStruct->payloadLength      = packet[PMM_NEO_PROTOCOL_INDEX_PAYLOAD_LENGTH];

            break;
    }
}

// Adds the corresponding header depending on the protocol.
// It assumes you already checked the total length of the packet to be sent.
// It returns the length of the chosen protocol header. If 0 is returned, an error ocurred.
uint8_t addProtocolHeader(uint8_t packet[], toBeSentTelemetryPacketInfoStructType* toBeSentTelemetryPacketInfoStruct)
{
    // 1) Test the given array
    if (!packet  || !toBeSentTelemetryPacketInfoStruct)    // If given array or struct is NULL, error!
        return 0;

    // 2) Which protocol are we using?
    switch (toBeSentTelemetryPacketInfoStruct->protocol)
    {

        // NEO
        case PMM_NEO_PROTOCOL_ID:

            // NEO, 3) Write the Protocol ID. These previous checks are to possibly make the code faster, as most of these
            // parameters doesn't change during runtime. Maybe I will remove them someday. Had to make a decision. No regrets.
            if (packet[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL] != PMM_NEO_PROTOCOL_ID)
                packet[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL]  = PMM_NEO_PROTOCOL_ID;
           
            // NEO, 4) Write the Source Address
            if (packet[PMM_NEO_PROTOCOL_INDEX_SOURCE] != toBeSentTelemetryPacketInfoStruct->sourceAddress)
                packet[PMM_NEO_PROTOCOL_INDEX_SOURCE]  = toBeSentTelemetryPacketInfoStruct->sourceAddress;
            
            // NEO, 5) Write the Destination Address
            if (packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] != toBeSentTelemetryPacketInfoStruct->destinationAddress); 
                packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION]  = toBeSentTelemetryPacketInfoStruct->destinationAddress; 

            // NEO, 6) Write the Port
            if (packet[PMM_NEO_PROTOCOL_INDEX_PORT] != toBeSentTelemetryPacketInfoStruct->port);
                packet[PMM_NEO_PROTOCOL_INDEX_PORT]  = toBeSentTelemetryPacketInfoStruct->port;

            // NEO, 7) Write the Payload Length
            if (packet[PMM_NEO_PROTOCOL_INDEX_PAYLOAD_LENGTH] != toBeSentTelemetryPacketInfoStruct->payloadLength);
                packet[PMM_NEO_PROTOCOL_INDEX_PAYLOAD_LENGTH]  = toBeSentTelemetryPacketInfoStruct->payloadLength;

            // NEO, 8) Write the CRC of this header
            uint16_t crcVar = crc16(packet, PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_LSB); // The length is the same as the index of the CRC LSB

            packet[PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_LSB] = LSB0(crcVar);   // Here we don't check the previous CRC as the CRC value isn't stored in the
            packet[PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_MSB] = LSB1(crcVar);   // toBeSentTelemetryPacketInfoStructType. Even if it was, any change on a parameter would change the CRC value,
                                                                            // so "probabilistically", maybe it won't worth the checking.

            return PMM_NEO_PROTOCOL_HEADER_LENGTH;
    }
}

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
    switch(packet[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        #if PMM_TELEMETRY_PROTOCOLS_ACCEPTS_NEO_PROTOCOL
            case PMM_NEO_PROTOCOL_ID:

                // NEO, 3.1) Checks the length of the packet again, now based on the protocol.
                if (packetLength < PMM_NEO_PROTOCOL_HEADER_LENGTH)
                    return 1; // Too short to be a real message

                // NEO, 3.2) Compares the length given by the Physical Layer (LoRa) and by the Transport Layer (Neo Protocol).
                if ((packetLength - PMM_NEO_PROTOCOL_HEADER_LENGTH) != packet[PMM_NEO_PROTOCOL_INDEX_PAYLOAD_LENGTH])
                    return 2;

                // NEO, 4) Checks the CRC.
                uint16_t crcVar = crc16(packet, packetLength);
                if ((LSB0(crcVar) != packet[PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_LSB]) || (LSB1(crcVar) != packet[PMM_NEO_PROTOCOL_INDEX_HEADER_CRC_MSB]))
                    return 3;

                // NEO, 5.1) Check if the Destination Address is to this device...
                if ((packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] != thisAddress) && !promiscuousMode)
                    return 4;

                // NEO, 5.2) ... and if is valid (there are forbidden addresses -- promiscuousMode isn't DUMB mode!)
                // Allowing forbidden addresses, like the PMM_TELEMETRY_ADDRESS_SELF, would mess the system / could be used for attacks.
                if (packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] >= PMM_TELEMETRY_ADDRESSES_INITIAL_FORBIDDEN_DESTINATION &&
                    packet[PMM_NEO_PROTOCOL_INDEX_DESTINATION] <= PMM_TELEMETRY_ADDRESSES_FINAL_FORBIDDEN_DESTINATION)
                    return 5;

                // NEO, 6) Check if the Source Address is valid (there are forbidden addresses).
                if (packet[PMM_NEO_PROTOCOL_INDEX_SOURCE] >= PMM_TELEMETRY_ADDRESSES_INITIAL_FORBIDDEN_SOURCE &&
                    packet[PMM_NEO_PROTOCOL_INDEX_SOURCE] <= PMM_TELEMETRY_ADDRESSES_FINAL_FORBIDDEN_SOURCE)
                    return 6;

                return 0;   // Successful.

        #endif

        default:
            return -2;   // Invalid Protocol.
    }

}
