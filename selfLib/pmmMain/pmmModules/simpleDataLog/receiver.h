// By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_SIMPLE_DATA_LOG_RX_h
#define PMM_MODULE_SIMPLE_DATA_LOG_RX_h

#include <stdint.h>

#include "pmmTelemetry/telemetry.h"                 // For transmitting
#include "pmmSd/sd.h"                               // For storing
#include "pmmModules/simpleDataLog/core.h"


// For now only allows 1 transmitter.
// To change it, you will have to have add a way to have multiple transmitterAddress.
// It's not hard at all. You can do it. I would, but there is no time!
class ModuleSimpleDataLogRx : public ModuleSimpleDataLogCore
{

public:
    ModuleSimpleDataLogRx();

    int  init(PmmSd* pmmSd, uint8_t systemSession, uint8_t sourceAddress);

    //    Returns true if received a valid packet.
    bool  receivedPacket(receivedPacketAllInfoStructType* packetInfo);

    

    // Also adds the Transmission counter
    int  addBasicInfo           (); 

    int  addMagnetometer        ();
    int  addGyroscope           ();
    int  addAccelerometer       ();
    int  addMpuTemperature      ();
    int  addBarometerPressure   ();
    int  addBarometerAltitude   ();
    int  addBmpTemperature      ();

    int  addGpsLatLong          ();
    int  addGpsAltitude         ();
    int  addGpsSatellites       ();

    // For a quick way to add a variable to the group. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like TYPE_ID_UINT8;
    int  addCustomVariable(const char *variableName, uint8_t variableType);

private:

    int     includeVariable(const char  variableName[], uint8_t variableType);

    // Should be runned after successful receivedPacket().
    int  storeOnSd(uint8_t sourceSession, bool writeOnBckupToo = true);

    uint8_t mSourceAddress;
    uint8_t mSessionId; // Id from this receiver system.

    uint8_t mVarsData[TLM_MAX_PAYLOAD_LENGTH];

    PmmSd  *mPmmSdPtr;

};

#endif