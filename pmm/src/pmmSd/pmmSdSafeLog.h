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
        uint32_t actualBlock;           // 4 B   = 4   B
        uint16_t relativeEndBlock;      // 1.7 B = 3 B 
        uint8_t  groupLength;           // 1 B   = 5   B
        uint8_t  actualPart;            
        uint16_t blocksPerPart;         //
        uint16_t positionInBlock : 9;   // As each block has 512 bytes, we need 
    } safeLogStatusStructType;

    PmmSdSafeLog(PmmSd *pmmSd, char* baseFilename, uint8_t sourceAddress, uint16_t blocksAllocationPerPart, uint8_t bufferSizeInBlocks, uint16_t dataLength);
    int write(uint8_t data[]);


private:

};