#include "pmm.h"

#define PMM_SD_EXTRA_CODES                       1
#if PMM_SD_EXTRA_CODES
    #include "pmmSd/extraCodes/main.h"
#endif

#define PMM_GPS_EXTRA_CODES                      1
#define PMM_IMU_EXTRA_CODES                      1
#define PMM_TELEMETRY_EXTRA_CODES   0

#if PMM_SD_EXTRA_CODES || PMM_GPS_EXTRA_CODES || PMM_IMU_EXTRA_CODES || PMM_TELEMETRY_EXTRA_CODES
    #define PMM_WILL_EXTRA_CODE     1
#endif







//Pmm pmm;

// Extra Codes are codes from Formatting the SD card to testing and calibrating sensors.
void runExtraCodes()
{

    #if PMM_WILL_EXTRA_CODE

        uint32_t serialDebugTimeout = millis();

        Serial.begin(9600);

        while (!Serial && ((millis() - serialDebugTimeout) < 5000));

        Serial.println("=-= PMM Extra Code initialized automatically. You can disable it at . =-=");

        #if PMM_SD_EXTRA_CODES
            PmmSdExtraCodes();
        #endif

    #endif
}

void setup()
{
    runExtraCodes();
    //pmm.init();
    //expansion.init();
}

void loop()
{
    //pmm.update();
    //expansion.update();

}
