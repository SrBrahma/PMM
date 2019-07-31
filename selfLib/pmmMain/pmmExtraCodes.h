/* pmmExtraCodes.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_EXTRA_CODES_h
#define PMM_EXTRA_CODES_h

#include "pmmConsts.h"  // For the defines

#if PMM_EXTRA_CODES_ENABLE && PMM_DEBUG && (PMM_EXTRA_CODES_SD_ENABLE || PMM_EXTRA_CODES_GPS_ENABLE || PMM_EXTRA_CODES_IMU_ENABLE || PMM_EXTRA_CODES_TLM_ENABLE)

    #if PMM_EXTRA_CODES_SD_ENABLE
        #include "pmmSd/extraCodes/main.h"
    #endif

    #if PMM_EXTRA_CODES_IMU_ENABLE
        #include "pmmImu/extraCodes/main.h"
    #endif

    #if PMM_EXTRA_CODES_TLM_ENABLE
        #include "pmmTelemetry/extraCodes/main.h"
    #endif

    // Extra Codes are codes from Formatting the SD card to testing and calibrating sensors.
    // This always returns true.
    bool runExtraCodes()
    {
        if (Serial) {
            Serial.println("=-= PMM Extra Codes initialized automatically. You can disable it at pmmConsts.h. =-=\n");

            #if PMM_EXTRA_CODES_SD_ENABLE
                PmmSdExtraCodes();
            #endif

            #if PMM_EXTRA_CODES_IMU_ENABLE
                PmmImuExtraCodes();
            #endif

            #if PMM_EXTRA_CODES_TLM_ENABLE
                PmmTelemetryExtraCodes();
            #endif

            Serial.println("=-= End of PMM Extra Codes. =-=");
        }

        return true;
    }

#else
    bool runExtraCodes()
    {
        return false;
    }

#endif // End of #if PMM_EXTRA_CODES_ENABLE

#endif // End of #ifndef PMM_EXTRA_CODES_h