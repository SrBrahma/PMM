/* pmmExtraCodes.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_EXTRA_CODES_h
#define PMM_EXTRA_CODES_h

#include "pmmConsts.h"  // For the defines

#if PMM_EXTRA_CODES_ENABLE
    #if PMM_SD_EXTRA_CODES || PMM_GPS_EXTRA_CODES || PMM_IMU_EXTRA_CODES || PMM_TLM_EXTRA_CODES
        #define PMM_EXTRA_CODES_WILL_HAPPEN
    #endif
#endif

#ifdef PMM_EXTRA_CODES_WILL_HAPPEN

    #if PMM_SD_EXTRA_CODES
        #include "pmmSd/extraCodes/main.h"
    #endif

    #if PMM_TLM_EXTRA_CODES
        #include "pmmTelemetry/extraCodes/main.h"
    #endif

    // Extra Codes are codes from Formatting the SD card to testing and calibrating sensors.
    // This always returns true.
    bool runExtraCodes()
    {

        uint32_t serialDebugTimeout = millis();

        Serial.begin(9600);

        while (!Serial && ((millis() - serialDebugTimeout) < PMM_DEBUG_TIMEOUT_MILLIS));

        if (Serial)
        {
            Serial.println("=-= PMM Extra Codes initialized automatically. You can disable it at pmmConsts.h. =-=\n");

            #if PMM_SD_EXTRA_CODES
                PmmSdExtraCodes();
            #endif

            #if PMM_TLM_EXTRA_CODES
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