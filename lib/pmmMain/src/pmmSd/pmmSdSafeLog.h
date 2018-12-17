/* pmmSdSafeLog.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_SAFE_LOG_h
#define PMM_SD_SAFE_LOG_h

#include <stdint.h>
#include <SdFat.h>

#include "pmmSd/pmmSd.h"
#include "pmmSd/pmmSdAllocation.h"

#define PMM_SD_SAFE_LOG_DEFAULT_PART_SIZE_KIB   1024

// typedef PmmSdAllocStatus PmmSdAllocStatus;

// System made to work with data of persistent length. Later I will explain it more.
// This need a deconstructor!
// The maximum buffer length is defined by PMM_SD_MAXIMUM_BUFFER_LENTH_KIB! If the given value is greater than this, will be replaced by this maximum!
// As the blocksAllocationPerPart is an uint16_t, the maximum file part size is 32MiB. As making bigger file parts doesn't seem too reasonable for the system specifications, will leave this way.
class PmmSdSafeLog : public PmmSdAllocation

{

public:

    PmmSdSafeLog(SdFatSdio* sdFat, SdioCard* sdioCard);

    int write(uint8_t data[], char dirFullRelativePath[], PmmSdAllocStatus* safeLogStatusStruct, uint8_t externalBlockBuffer[PMM_SD_BLOCK_SIZE] = NULL); // Know what you are doing!

    const char* getFilenameExtension();

    int getNumberFileParts(char dirFullRelativePath[], uint8_t* fileParts);
    int getFileRange(char dirFullRelativePath[], uint8_t filePart, uint32_t *beginBlock, uint32_t *endBlock);

private:

    SdFatSdio* mSdFat;
    SdioCard*  mSdioCard;

    static constexpr const char* PMM_SD_SAFE_LOG_FILENAME_EXTENSION = "slog"; // https://stackoverflow.com/a/25323360/10247962

    uint8_t mBlockBuffer[PMM_SD_BLOCK_SIZE];


};

#endif