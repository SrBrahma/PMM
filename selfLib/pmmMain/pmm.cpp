// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)

#include "pmmConsts.h"
#include "pmmExtraCodes.h"
#include "pmmDebug.h"
#include "pmm.h"


Pmm::Pmm() {}



void Pmm::init()
{
    initDebug();     // No need to comment this function. To disable it, change PMM_DEBUG on pmmConsts.h
    runExtraCodes(); // No need to comment this function. To disable it, change PMM_EXTRA_CODES_ENABLE on pmmConsts.h
    //i2cScan();       // Comment it if not doing a i2c scan!

    // The object mRoutine is specified in the pmm.h automatically, based on the routine defined in the pmmConsts.h .
    mRoutine.init();

}



void Pmm::update()
{
    mRoutine.update();
}



void initDebug()
{
    #if PMM_DEBUG   // Debug
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to its maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print

        #if PMM_DEBUG_TIMEOUT_ENABLED
            uint32_t serialDebugTimeout = millis();
            while (!Serial && ((millis() - serialDebugTimeout) < PMM_DEBUG_TIMEOUT_MILLIS));
        #else
            while (!Serial);
        #endif

        if (Serial)
            debugMorePrintf("Serial initialized!\n");
    #endif
}





// These below are no longer used, for now.
// // Be careful using the functions below. You can easily turn the PMM code into a hell by bypassing the default running order.
// PmmImu*              Pmm::getPmmImuPtr()            { return &mPmmImu;               }
// PmmTelemetry*        Pmm::getTelemetryPtr()         { return &mPmmTlm;         }
// PmmGps*              Pmm::getGpsPtr()               { return &mPmmGps;               }
// PmmSd*               Pmm::getSdPtr()                { return &mPmmSd;                }
// PmmModuleDataLog*    Pmm::getModuleDataLog()        { return &mPmmModuleDataLog;     }
// PmmModuleMessageLog* Pmm::getModuleMessageLog()     { return &mPmmModuleMessageLog;  }




// void i2cScan() // https://gist.github.com/tfeldmann/5411375
// {
//     Wire2.begin();
//     byte error, address;
//     int nDevices = 0;

//     Serial.println("Scanning...");

//     for(address = 1; address < 127; address++ ) 
//     {
//         // The i2c_scanner uses the return value of
//         // the Write.endTransmisstion to see if
//         // a device did acknowledge to the address.
//         Wire2.beginTransmission(address);
//         error = Wire2.endTransmission();

//         if (error == 0)
//         {
//             Serial.print("I2C device found at address 0x");
//             if (address<16) 
//                 Serial.print("0");
//             Serial.print(address,HEX);
//             Serial.println("  !");

//             nDevices++;
//         }
//         else if (error==4) 
//         {
//             Serial.print("Unknow error at address 0x");
//             if (address<16) 
//                 Serial.print("0");
//             Serial.println(address,HEX);
//         }    
//     }
//     if (nDevices == 0)
//         Serial.println("No I2C devices found\n");
//     else
//         Serial.println("done\n");
// }