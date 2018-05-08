#include <pmmTelemetry.h>


TelemetryManager::TelemetryManager()
{
}

int
TelemetryManager::updateReceive()
{
    if (rf95.recv2(rfPayload))
    {
        if (!memcmp(rfPayload, RF_HEADER_LOG, 4))
        {
            // save in bin
            // save in .csv
            return 1;
        }
        else if (!memcmp(rfPayload, RF_HEADER_EXTRA_LOG, 4))
        {
            // save in txt

            return 1;
        }
        else if (!memcmp(rfPayload, RF_HEADER_VARS_INFO, 4))
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}
