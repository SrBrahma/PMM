#include "pmm.h"
#include "pmmExtraCodes.h"

Pmm pmm;

void setup()
{
    bool skipPmmDebugDelay;

    skipPmmDebugDelay = runExtraCodes(); // No need to comment this function. To disable it, change PMM_EXTRA_CODES_ENABLE on pmmConsts.h

    pmm.init(skipPmmDebugDelay);
    //expansion.init();
}

void loop()
{
    pmm.update();
    //expansion.update();

}
