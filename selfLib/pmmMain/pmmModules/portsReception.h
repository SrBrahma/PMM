/* portsReception.cpp
 * Directs the received package to the respective Extension.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_PORTS_RECEPTION_h
#define PMM_PORTS_RECEPTION_h

#include "pmmTelemetry/protocols.h"

// #include "pmmModules/dataLog/dataLog.h"
// #include "pmmModules/messageLog/messageLog.h"


class PmmPortsReception
{
public:
    PmmPortsReception();

    void addSimpleDataLog(PmmModuleSimpleDataLog *moduleSimpleDataLog);
    void receivedPacket(receivedPacketAllInfoStructType* PacketInfo);

private:
    PmmModuleSimpleDataLog *mModuleSimpleDataLog = NULL;
    // PmmModuleDataLog   * mPmmModuleDataLog;
    // PmmModuleMessageLog* mPmmModuleMessageLog;

};

#endif
