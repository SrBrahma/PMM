#ifndef PMM_PORTS_h
#define PMM_PORTS_h

#define PORT_ID_NONE                0x00 // None!
#define PORT_ID_DATA_LOG            0x01
#define PORT_ID_DATA_LOG_INFO       0x02
#define PORT_ID_MESSAGE_LOG         0x03
#define PORT_ID_REQUEST             0x04
#define PORT_ID_SIMPLE_DATA_LOG     0x05


#include "pmmModules/simpleDataLog/receiver.h"
// #include "pmmModules/dataLog/dataLog.h"
// #include "pmmModules/messageLog/messageLog.h"



class PortsReception
{

public:

    PortsReception() {}

    // As only the Rx receives, we don't need the Tx here!
    void addSimpleDataLogRx(ModuleSimpleDataLogRx *moduleSimpleDataLog);

    int receivedPacket(receivedPacketAllInfoStructType* packetInfo) {
        switch(packetInfo->port) {// 1) Which kind of packet is it?
            // case PORT_ID_DATA_LOG:
            //     mPmmModuleDataLog->receivedDataLog(packetInfo); break;
            // case PORT_ID_DATA_LOG_INFO:
            //     mPmmModuleDataLog->receivedDataLogInfo(packetInfo); break;
            // case PORT_ID_MESSAGE_LOG:
            //     mPmmModuleMessageLog->receivedPackageString(packetInfo); break;
            // case PORT_ID_REQUEST:
            //     return;
            case PORT_ID_SIMPLE_DATA_LOG:
                if (mModuleSimpleDataLog)
                    return mModuleSimpleDataLog->receivedPacket(packetInfo);
            default:;
        }
        return 0;
    }

private:
    ModuleSimpleDataLogRx *mModuleSimpleDataLog = NULL;
    // PmmModuleDataLog   * mPmmModuleDataLog;
    // PmmModuleMessageLog* mPmmModuleMessageLog;

};

#endif
