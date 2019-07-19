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

    int  init();

    // Returns true if received anything, else, false.
    bool updateReception();

    // Returns true if you can send a packet *right now*. False if you can't. Read the other funcs description below.
    bool isSendAvailable();

    // Sends the packet. Will wait a previous packet being sent, and will wait if is detected channel activity.
    // Basically, it WILL send. Can take some time waiting (nothing else on you code will happen!
    //                                                            Advanced info: Yield() happens.
    // Returns 0 if successfully sent. Else, error.
    int send(PacketToBeSent *packet);
   
    // Sends the packet, if the sending is available *right now*. If is detected that
    // a previous packet still being sent, or detected channel activity, it won't wait.
    // You should also run isSendAvaible() before this function, to know beforehand that building
    // the packet will be useless..
    // Returns 0 if successfully sent. Else, error or wasn't sent.
    int sendIfAvailable(PacketToBeSent *packet);

    receivedPacketAllInfoStructType* getRxPacketAllInfoPtr();


private:

    RH_RF95  mRf95;
    bool     mTelemetryIsWorking;

    receivedPacketPhysicalLayerInfoStructType   mRxPacketPhysicalLayerInfo;
    receivedPacketAllInfoStructType             mRxPacketAllInfo          ;

    #if PMM_TLM_SIMULATE_RECEPTION
        int    mNewSimulatedPacket;
    #endif
};


#endif
