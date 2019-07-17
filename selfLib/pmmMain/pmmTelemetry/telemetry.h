/* pmmTelemetry.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_TLM_h
#define PMM_TLM_h

#include <RH_RF95.h>                            // Our current RF module!
#include "pmmTelemetry/protocols.h" // For the PMM_NEO_PROTOCOL_HEADER_LENGTH define



class PmmTelemetry
{
public:
    PmmTelemetry();

    int init();

    int updateReception();

    bool isSendAvailable();
    int send(PacketToBeSent* packetToBeSent);
   


    receivedPacketAllInfoStructType* getReceivedPacketAllInfoStructPtr();


private:

    RH_RF95  mRf95;
    int      mTelemetryIsWorking;

    receivedPacketPhysicalLayerInfoStructType   mReceivedPacketPhysicalLayerInfoStruct;
    receivedPacketAllInfoStructType             mReceivedPacketAllInfoStruct          ;

    #if PMM_TLM_SIMULATE_RECEPTION
        int    mNewSimulatedPacket;
    #endif
};


#endif
