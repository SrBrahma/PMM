/* PmmPortLog.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */


#include <pmmTelemetryPorts/pmmPortsReception.h>
#include <pmmTelemetryPorts/pmmPortLog.h>
#include <pmmTelemetryPorts/pmmPortString.h>
#include <pmmTelemetryProtocols.h> // in RadioHead directory.

PmmPortsReception::PmmPortsReception()
{
}

int PmmPortsReception::init(PmmPortLog* PmmPortLog, PmmPortString* PmmPortString)
{
    mPmmPortLog = PmmPortLog;
    mPmmPortString = PmmPortString;
    return 0;
}

void PmmPortsReception::receivedPacket(uint8_t packetArray[], uint16_t packetLength)
{
    // 1) Which kind of packet is it?
    switch(packetArray[PMM_TELEMETRY_PROTOCOLS_INDEX_PROTOCOL])
    {
        case PMM_PORT_LOG:
            mPmmPortLog->receivedPackageLog(packetArray, packetLength);
            return;
        case PMM_PORT_LOG_INFO:
            mPmmPortLog->receivedPackageLogInfo(packetArray, packetLength);
            return;
        case PMM_PORT_STRING:
            mPmmPortString->receivedPackageString(packetArray, packetLength);
            return;
        case PMM_PORT_REQUEST:
            return;
    }
}
