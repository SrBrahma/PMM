/* portsReception.cpp
 * Directs the received package to the respective Extension.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "pmmModules/ports.h"
#include "pmmModules/portsReception.h"
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h" // To know how to decompose the Port field from the received packet


PmmPortsReception::PmmPortsReception()
{
}

int PmmPortsReception::init(PmmModuleDataLog* pmmPackageDataLog, PmmModuleMessageLog* pmmPackageMessageLog)
{
    mPmmModuleDataLog = pmmPackageDataLog;
    mPmmModuleMessageLog = pmmPackageMessageLog;
    return 0;
}

void PmmPortsReception::receivedPacket(uint8_t payload[], telemetryPacketInfoStructType* packetStatus)
{
    // 1) Which kind of packet is it?
    switch(payload[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        case PMM_PORT_DATA_LOG:
            mPmmModuleDataLog->receivedDataLog(payload, packetStatus);
            return;
        case PMM_PORT_LOG_INFO:
            mPmmModuleDataLog->receivedLogInfo(payload, packetStatus);
            return;
        case PMM_PORT_MESSAGE_LOG:
            mPmmModuleMessageLog->receivedPackageString(payload, packetStatus);
            return;
        case PMM_PORT_REQUEST:
            return;
    }
}
