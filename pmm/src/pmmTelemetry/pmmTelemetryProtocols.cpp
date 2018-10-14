#include <stdint.h>
#include "pmmTelemetry/pmmTelemetryProtocols.h"

// if the given protocol is invalid, will by default use the Neo Protocol.
uint8_t getProtocolHeaderLength(uint8_t protocol)
{
    switch (protocol)
    {
        default:
        case PMM_NEO_PROTOCOL_ID:
            return PMM_NEO_PROTOCOL_HEADER_LENGTH;
    }
}



// Check the packet protocol and return the length of the header. If 0 is returned, the packet is invalid.
// It also checks if the Destination Address of the received packet is the same as the address of this system.
uint8_t validateReceivedPacketAndReturnProtocolHeaderLength(uint8_t packetData[], uint8_t bufferLength, uint8_t thisAddress, int promiscuousMode)
{
    // 1) Which protocol is this packet using?
    switch(packetData[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        #if PMM_TELEMETRY_PROTOCOLS_ACCEPTS_NEO_PROTOCOL
            case PMM_NEO_PROTOCOL_ID:
                //1.1) Test the packet length
                if (bufferLength < PMM_NEO_PROTOCOL_HEADER_LENGTH)
                    return 0; // Too short to be a real message

                // 1.2) Check the Destination address of this packet
                // 1.2.1) Is the Destination address valid?
                if (packetData[PMM_NEO_PROTOCOL_INDEX_DESTINATION] >= RH_INITIAL_FORBIDDEN_DESTINATION_ADDRESS &&
                    packetData[PMM_NEO_PROTOCOL_INDEX_DESTINATION] <= RH_FINAL_FORBIDDEN_DESTINATION_ADDRESS)
                    return 0;

                // 1.2.2) Is this system address the Destination of this packet, or are we in in promiscuous mode?
                if ((packetData[PMM_NEO_PROTOCOL_INDEX_DESTINATION] != thisAddress) && !promiscuousMode)
                    return 0;

                // 1.3) Check the Source address of this packet
                if (packetData[PMM_NEO_PROTOCOL_INDEX_SOURCE] >= RH_INITIAL_FORBIDDEN_SOURCE_ADDRESS &&
                    packetData[PMM_NEO_PROTOCOL_INDEX_SOURCE] <= RH_FINAL_FORBIDDEN_SOURCE_ADDRESS)
                    return 0;

                return PMM_NEO_PROTOCOL_HEADER_LENGTH;
        #endif

        default:
            return 0;
    }

}
