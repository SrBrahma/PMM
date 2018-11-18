/* pmmSdSafeLog.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <stdint.h>
#include <SdFat.h>

#include "pmmSd/pmmSd.h"


// System made to work with data of persistent length. Later I will explain it more.
// This need a deconstructor!
// The maximum buffer length is defined by PMM_SD_MAXIMUM_BUFFER_LENTH_KIB! If the given value is greater than this, will be replaced by this maximum!
// As the blocksAllocationPerPart is an uint16_t, the maximum file part size is 32MiB. As making bigger file parts doesn't seem too reasonable for the system specifications, will leave this way.
class PmmSdSafeLog
{

public:

    typedef struct
    {
        uint32_t actualBlock;           // At which block we currently are
        uint16_t freeBlocks;            // The absoluteEndBlock is actualBlock + freeBlocks.
        uint8_t  groupLength;           // Max group length is 255 bytes. On the future it may be extended. But not needed right now.
        uint8_t  actualPart;            // Maximum of 255 parts. Just a quicker way to get the next part filename instead of reading each part's filename.
        uint16_t blocksPerPart;         // max of 65535 blocks, maximum allocation per part is 32 MiB (64KiB * 512B)
        uint16_t positionInBlock : 9;   // As each block has 512 bytes, we need 9 bits.
    } safeLogStatusStructType;          // Total struct size is 12 bytes, with padding.

    PmmSdSafeLog(PmmSd *pmmSd, uint8_t sourceAddress, uint16_t blocksAllocationPerPart);

    int write(uint8_t data[], safeLogStatusStructType* safeLogStatusStruct, char dirFullRelativePath[]); // Know what you are doing!

    int writeOwn(uint8_t data[], safeLogStatusStructType* safeLogStatusStruct, char dirRelativePath[]);

    int writeReceived(uint8_t data[], safeLogStatusStructType* safeLogStatusStruct, uint8_t sourceAddress, uint8_t sourceSession, char dirRelativePath[]);

    
private:

    char mDirFullRelativePath[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];

    // Returns how many bytes from the data were written. Returns a negative number if error found.
    // Returns 0 if no data was written, but without any error -- just no space was available on the actual block.
    int writeOnBlock(uint8_t data[], uint8_t dataLength, safeLogStatusStructType* safeLogStatusStruct);
};