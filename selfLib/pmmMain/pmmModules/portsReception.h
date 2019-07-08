/* portsReception.cpp
 * Directs the received package to the respective Extension.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_PORTS_RECEPTION_h
#define PMM_PORTS_RECEPTION_h

#include "pmmTelemetry/protocols.h"
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"


class PmmPortsReception
{

private:

    PmmModuleDataLog   * mPmmModuleDataLog;
    PmmModuleMessageLog* mPmmModuleMessageLog;

public:
    PmmPortsReception();
    int init(PmmModuleDataLog* PmmModuleDataLog, PmmModuleMessageLog* PmmModuleMessageLog);
    void receivedPacket(receivedPacketAllInfoStructType* PacketInfo);

}; // End of the class

#endif
