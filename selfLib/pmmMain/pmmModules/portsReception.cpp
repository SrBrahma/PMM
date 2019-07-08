/* portsReception.cpp
 * Directs the received package to the respective Extension.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "pmmModules/ports.h"
#include "pmmTelemetry/protocols.h" // To know how to decompose the Port field from the received packet

#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"

#include "pmmModules/portsReception.h"


PmmPortsReception::PmmPortsReception()
{
}

int PmmPortsReception::init(PmmModuleDataLog* pmmPackageDataLog, PmmModuleMessageLog* pmmPackageMessageLog)
{
    mPmmModuleDataLog = pmmPackageDataLog;
    mPmmModuleMessageLog = pmmPackageMessageLog;
    return 0;
}

void PmmPortsReception::receivedPacket(receivedPacketAllInfoStructType* packetInfo)
{
    // 1) Which kind of packet is it?
    switch(packetInfo->port)
    {
        case PORT_DATA_LOG_ID:
            mPmmModuleDataLog->receivedDataLog(packetInfo);
            return;
        case PORT_DATA_LOG_INFO_ID:
            mPmmModuleDataLog->receivedDataLogInfo(packetInfo);
            return;
        case PORT_MESSAGE_LOG_ID:
            mPmmModuleMessageLog->receivedPackageString(packetInfo);
            return;
        case PORT_REQUEST_ID:
            return;
    }
}
