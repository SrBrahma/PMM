// By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_MODULE_SIMPLE_DATA_LOG_TX_h
#define PMM_MODULE_SIMPLE_DATA_LOG_TX_h

#include "pmmTelemetry/telemetry.h"                 // For transmitting
#include "pmmSd/sd.h"                               // For storing
#include "pmmModules/simpleDataLog/core.h"



class ModuleSimpleDataLogTx : private ModuleSimpleDataLogCore
{

public:

    ModuleSimpleDataLogTx();

    // The pmmTelemetry can be NULL, which tell this object that it won't transmit.
    int  init(PmmTelemetry* pmmTelemetry, PmmSd* pmmSd, uint8_t systemSession);

    // Transmission
    int  send(uint8_t destinationAddress = PMM_TLM_ADDRESS_BROADCAST);

    // Storage
    int  storeOnSd(bool writeOnBckupToo = true);

    // Print the variables names
    void printHeader ();

    // Print the variables values
    void printContent(bool printHeadersTogether = true);


        // Also adds the Transmission counter
    int  addBasicInfo            (uint32_t* mainLoopCounterPtr, uint32_t* timeMsPtr); 

    int  addMagnetometer         (float     magnetometerArray []);
    int  addGyroscope            (float     gyroscopeArray    []);
    int  addAccelerometer        (float     accelerometerArray[]);
    int  addMpuTemperature       (float    *mpuTemperaturePtr   );
    int  addBarometerPressure    (float    *barometerPressurePtr);
    int  addBarometerAltitude    (float    *barometerAltitudePtr);
    int  addBmpTemperature       (float    *bmpTemperaturePtr   );

    int  addGpsLatLong           (int32_t *latitude, int32_t *longitude);
    int  addGpsLastLocationTimeMs(uint32_t *lastLocationTimeMs);
    int  addGpsAltitude          (float    *altitude           );
    int  addGpsSatellites        (uint8_t  *satellites         );

    // For a quick way to add a variable to the group. Make sure the given variable name and the variable itself is static, global,
    // "const PROGMEM", or any other way that the variable isn't lost during the program run. Variable type follows the #define's like TYPE_ID_UINT8;
    int  addCustomVariable(const char *variableName, uint8_t variableType, void *variableAddress);
    

private:

    int     includeVariable(const char  variableName[],   uint8_t variableType, void* variableAddress);

    // The 
    bool    mUsesTelemetry;

    bool    mIsFirstStoreOnSd;

    File    mFile, mFile2;
    char    mFilePath[PMM_SD_FILENAME_MAX_LENGTH], mFilePath2[PMM_SD_FILENAME_MAX_LENGTH];

    PmmTelemetry*  mPmmTlmPtr;
    PmmSd       *  mPmmSdPtr;

    uint8_t  mSystemSession;
    uint32_t mTxCounter;

};

#endif
