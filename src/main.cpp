#include "pmm.h"

#define PMM_SD_QUICK_TEST                       1
#define PMM_GPS_QUICK_TEST                      1
#define PMM_IMU_QUICK_TEST                      1
#define PMM_TELEMETRY_TRANSMISSION_QUICK_TEST   0
#define PMM_TELEMETRY_RECEPTION_QUICK_TEST      0


#if PMM_SD_QUICK_TEST ^ PMM_GPS_QUICK_TEST ^ PMM_IMU_QUICK_TEST //^ PMM_TELEMETRY_TRANSMISSION_QUICK_TEST || PMM_TELEMETRY_RECEPTION_QUICK_TEST
    #define PMM_WILL_QUICK_TEST_SOMETHING
#endif
Pmm pmm;

void setup()
{
    pmm.init();
    //expansion.init();
}

void loop()
{
    pmm.update();
    //expansion.update();

}
