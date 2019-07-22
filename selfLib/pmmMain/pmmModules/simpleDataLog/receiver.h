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

    // The sourceAddres is the address of the transmitter, the one who you want to listen to.
    int  init(PmmSd* pmmSd, uint8_t systemSession, uint8_t sourceAddress);

    // Returns true if received a valid packet.
    bool receivedPacket(receivedPacketAllInfoStructType* packetInfo, bool autoStoreOnSd = false);

    // Stores the last received packet on the SD.
    // Should be runned after successful receivedPacket(). (can be automatically called using
    // autoStoreOnSd, on receivedPacket arg.
    int  storeOnSd(uint8_t sourceSession, bool writeOnBckupToo = true);

    // Returns the index of the variable, to be used on getVarByIndex function.
    // You can get the varExactName on pmmModules/simpleDataLog/varsNames.h,
    // or by calling this receiver object with 'mReceiver'.mStr.'names!', where the fields between ' ' are
    // your choices! Only yours!
    // Returns -1 if not found.
    int  getVarIndex(const char varExactName[]);

    // Copies to the first arg the var value of the last received packet. Be sure before that the types match.
    // It won't know if there was already a received packet. You should check the return value of
    // receivedPacket before. The index you can get by using getVarIndex function.
    // Returns -1 if null destination, -2 if invalid index.
    int  getVarByIndex(void *destination, int index);

    // You can add other types.
    // The values are updated after a successful receivedPacket();
    // Be sure that the given index is valid. If invalid, 0 is returned.
    uint8_t  getVar_uint8_t (int index);
    int32_t  getVar_int32_t (int index);
    uint32_t getVar_uint32_t(int index);
    float    getVar_float   (int index);

    // Get the session of the transmitter.
    // The value is updated after a successful receivedPacket();
    uint8_t  getSourceSession();


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
    int  addGpsLastLocationTimeMs();
    int  addGpsAltitude         ();
    int  addGpsSatellites       ();

    // For a quick way to add a variable to the group. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like TYPE_ID_UINT8;
    int  addCustomVariable(const char *variableName, uint8_t variableType);

private:

    int     includeVariable(const char  variableName[], uint8_t variableType);

    uint8_t mSourceAddress;
    uint8_t mSourceSession;

    uint8_t mSessionId; // Id from this receiver system.

    uint8_t mVarsData[TLM_MAX_PAYLOAD_LENGTH];

    PmmSd  *mPmmSdPtr;

};

#endif