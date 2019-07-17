#include "pmmModules/dataLog/dataLogGroupCore.h"
#include "pmmModules/dataLog/dataLog.h"



int getDataLogDirectory(char destination[], uint8_t maxLength, uint8_t dataLogId, uint8_t groupLength, const char additionalPath[])
{
    if (!destination) return 0;

    if (!additionalPath || additionalPath[0] == '\0') // %hu or %u? https://stackoverflow.com/a/8702610/10247962
        return snprintf(destination, maxLength, "DataLog ID-%u L-%u", dataLogId, groupLength);

    else
        return snprintf(destination, maxLength, "DataLog ID-%u L-%u/%s", dataLogId, groupLength, additionalPath);
}