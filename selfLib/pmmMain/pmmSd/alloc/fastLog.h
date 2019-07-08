/* pmmSdSafeLog.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

// Commented this code for a while.

/*

#include <stdint.h>
#include <SdFat.h>

#include "pmmSd/sd.h"



#define PMM_SD_MAXIMUM_BUFFER_LENTH_KIB     16

// System made to work with data of persistent length. Later I will explain it more.
// This need a deconstructor!
// The maximum buffer length is defined by PMM_SD_MAXIMUM_BUFFER_LENTH_KIB! If the given value is greater than this, will be replaced by this maximum!
// As the blocksAllocationPerPart is an uint16_t, the maximum file part size is 32MiB. As making bigger file parts doesn't seem too reasonable for the system specifications, will leave this way.
class PmmSdFastLog
{

public:
    PmmSdFastLog(PmmSd *pmmSd, uint16_t dataLength, char dirFullRelativePath[], uint16_t blocksAllocationPerPart, uint8_t bufferSizeInBlocks);
    int write(uint8_t data[]);


private:
    int flush();

    // File informations
    char        mFilenameExtension[PMM_SD_FILENAME_SUFFIX_LENGTH] = ".flog";    // Must include the '.' (dot)! If no extensions is given, the file won't have one!
    char        mBaseFilename[PMM_SD_FILENAME_MAX_LENGTH];               // The code will include the current part to its name, so "name" will be "name_00", for example.
    uint8_t     mSourceAddress;


    // Blocks!
    uint16_t    mBlocksAllocationPerPart;       // How many blocks of 512 bytes the file will have? A 1MiB file have (512 * 2 * 1024) =
    uint16_t    mActualBlockInPart;
    uint32_t    mActualBlockAddress;


    // Parts!
    uint8_t     mCurrentNumberOfParts;


    // Buffer!
    uint8_t*    mBufferPointer; // The array is malloc()'ed at the object initializer. This pointer points to the malloc()'ed array!

    uint16_t    mBufferSizeInBlocks;            // A buffer of this*512 bytes will be created. Bigger buffer may not necessarily means a faster write rate. Test it.

    uint16_t    mBufferTotalLength; // Being uint16_t, limits the buffer to have a maximum length of 65536; 64KiB. We will NEVER use it. (in 30 years, this comment will be funny)
    uint16_t    mBufferActualIndex;

    uint16_t    mDataLength;

    PmmSd*      mPmmSdPtr;
    File        mFile;
};

*/