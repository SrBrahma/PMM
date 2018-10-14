/* PmmPackageDataLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */


#include "pmmPackages/portsReception.h"
#include "pmmPackages/dataLog/dataLog.h"
#include "pmmPackages/messageLog/messageLog.h"
#include "pmmTelemetry/pmmTelemetryProtocols.h"


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
        case PMM_PORT_LOG:
            mPmmPackageDataLog->receivedPackageLog(payload, packetStatus);
            return;
        case PMM_PORT_LOG_INFO:
            mPmmPackageDataLog->receivedPackageLogInfo(payload, packetStatus);
            return;
        case PMM_PORT_STRING:
            mPmmPackageMessageLog->receivedPackageString(payload, packetStatus);
            return;
        case PMM_PORT_REQUEST:
            return;
    }
}
