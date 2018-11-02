/* PmmPackageDataLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include "pmmPackages/ports.h"
#include "pmmPackages/portsReception.h"
#include "pmmPackages/dataLog/dataLog.h"
#include "pmmPackages/messageLog/messageLog.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h" // To know how to decompose the Port field from the received packet


PmmPortsReception::PmmPortsReception()
{
}

int PmmPortsReception::init(PmmPackageDataLog* pmmPackageDataLog, PmmPackageMessageLog* pmmPackageMessageLog)
{
    mPmmPackageDataLog = pmmPackageDataLog;
    mPmmPackageMessageLog = pmmPackageMessageLog;
    return 0;
}

void PmmPortsReception::receivedPacket(uint8_t payload[], telemetryPacketInfoStructType* packetStatus)
{
    // 1) Which kind of packet is it?
    switch(payload[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        case PMM_PORT_DATA_LOG:
            mPmmPackageDataLog->receivedDataLog(payload, packetStatus);
            return;
        case PMM_PORT_LOG_INFO:
            mPmmPackageDataLog->receivedLogInfo(payload, packetStatus);
            return;
        case PMM_PORT_MESSAGE_LOG:
            mPmmPackageMessageLog->receivedPackageString(payload, packetStatus);
            return;
        case PMM_PORT_REQUEST:
            return;
    }
}
