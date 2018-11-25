/* pmmSdSafeLog.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <stdint.h>
#include <SdFat.h>

#include "pmmSd/pmmSd.h"
#include "pmmSd/pmmSdAllocation.h"

#define PMM_SD_SAFE_LOG_EXTENSION       "slog" // The extension added to the file parts. The '.' is automatically added.


// System made to work with data of persistent length. Later I will explain it more.
// This need a deconstructor!
// The maximum buffer length is defined by PMM_SD_MAXIMUM_BUFFER_LENTH_KIB! If the given value is greater than this, will be replaced by this maximum!
// As the blocksAllocationPerPart is an uint16_t, the maximum file part size is 32MiB. As making bigger file parts doesn't seem too reasonable for the system specifications, will leave this way.
class PmmSdSafeLog : public PmmSdAllocation

{

public:

    PmmSdSafeLog(SdFatSdio* sdFat, uint16_t defaulBlocksAllocationPerPart);

    // If the blocksPerPart is 0, as is the default argument, the mDefaultKiBAllocationPerPart variable value will be used.
    void initSafeLogStatusStruct(pmmSdAllocationStatusStructType* safeLogStatusStruct, uint8_t groupLength, uint16_t KiBPerPart = 0);

    int write(uint8_t data[], char dirFullRelativePath[], pmmSdAllocationStatusStructType* safeLogStatusStruct); // Know what you are doing!

private:

    SdFatSdio* mSdFat;

    uint16_t mDefaultKiBAllocationPerPart;

    uint8_t mBlockBuffer[PMM_SD_BLOCK_SIZE];

};