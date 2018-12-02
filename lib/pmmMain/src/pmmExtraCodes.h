/* pmmExtraCodes.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_EXTRA_CODES_h
#define PMM_EXTRA_CODES_h

#include "pmmConsts.h"

#if PMM_EXTRA_CODES_ENABLE

    #define PMM_SD_EXTRA_CODES                       1

    #if PMM_SD_EXTRA_CODES
        #include "pmmSd/extraCodes/main.h"
    #endif


    #define PMM_GPS_EXTRA_CODES                      1
    #define PMM_IMU_EXTRA_CODES                      1
    #define PMM_TELEMETRY_EXTRA_CODES   0



    // Extra Codes are codes from Formatting the SD card to testing and calibrating sensors.
    void runExtraCodes()
    {

        #if PMM_SD_EXTRA_CODES || PMM_GPS_EXTRA_CODES || PMM_IMU_EXTRA_CODES || PMM_TELEMETRY_EXTRA_CODES

            uint32_t serialDebugTimeout = millis();

            Serial.begin(9600);

            while (!Serial && ((millis() - serialDebugTimeout) < 10000));

            Serial.println("=-= PMM Extra Code initialized automatically. You can disable it at pmmConsts.h. =-=\n");

            #if PMM_SD_EXTRA_CODES
                PmmSdExtraCodes();
            #endif

        #endif
    }

#endif // End of #if PMM_EXTRA_CODES_ENABLE

#endif // End of #ifndef PMM_EXTRA_CODES_h