/* pmmSdAllocation.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <string.h>                 // For snprintf

#include "pmmDebug.h"
#include "pmmSd/consts.h"
#include "pmmSd/alloc/alloc.h"


PmmSdAllocation::PmmSdAllocation(SdFatSdio* sdFat)
{
    mSdFat = sdFat;
    mSdioCard = sdFat->card();
}


int PmmSdAllocation::getFilePartName(char destinationArray[], char dirFullRelativePath[], uint8_t filePart, const char filenameExtension[])
{
    if (!destinationArray)    { advPrintf("No destination given\n"); return 1; }
    if (!dirFullRelativePath) { advPrintf("No directory given\n");   return 2; }
    if (!filenameExtension)   { advPrintf("No extension given\n");   return 3; }

    snprintf(destinationArray, PMM_SD_FILENAME_MAX_LENGTH, "%s/part-%u.%s", dirFullRelativePath, filePart, filenameExtension);

    return 0;
}

// Returns by reference the number of actual file parts.
// Returns 0 if no error.
int PmmSdAllocation::getNumberFileParts(char dirFullRelativePath[], const char filenameExtension[], uint8_t* fileParts)
{
    char filename[PMM_SD_FILENAME_MAX_LENGTH];

    if (!dirFullRelativePath || !filenameExtension || !fileParts) // invalid addresses
        return 1;

    *fileParts = 0;

    while (1)
    {
        getFilePartName(filename, dirFullRelativePath, *fileParts, filenameExtension);
        if (mSdFat->exists(filename))
            (*fileParts)++;
        else
            break;
    }

    return 0;
}


int PmmSdAllocation::getFileRange(char filePath[], uint32_t *beginBlock, uint32_t *endBlock)
{
    if (!mFile.open(filePath, O_READ))
        return 1; // File doesn't exist.

    mFile.contiguousRange(beginBlock, endBlock);
    mFile.close();

    return 0;
}


int PmmSdAllocation::nextBlockAndAllocIfNeeded(char dirFullRelativePath[], const char filenameExtension[], PmmSdAllocStatus* allocStatus)
{
    // 1) Do we need a new part? No!
    if (allocStatus->freeBlocksAfterCurrent)
    {
        allocStatus->freeBlocksAfterCurrent--; // We don't need a new part!
        allocStatus->currentBlock++;
        return 0;
    }

    // 2) YA
    // NOTE this function does currentPositionInBlock = 0; freeBlocksAfterCurrent = endBlock - allocStatus->currentBlock; nextFilePart++
     return allocateFilePart(dirFullRelativePath, filenameExtension, allocStatus);
}


// Handles our PmmSdAllocStatus struct automatically.
int PmmSdAllocation::allocateFilePart(char dirFullRelativePath[], const char filenameExtension[], PmmSdAllocStatus* allocStatus)
{
    uint32_t endBlock;
    int returnValue;

    if (!(returnValue = allocateFilePart(dirFullRelativePath, filenameExtension, allocStatus->nextFilePart, allocStatus->KiBPerPart, &allocStatus->currentBlock, &endBlock)))
    {
        allocStatus->freeBlocksAfterCurrent = endBlock - allocStatus->currentBlock;
        allocStatus->nextFilePart++;
    }

    return returnValue;
}


// Allocates a file part with a length of X blocks.
// If no problems found, return 0.
// The filenameExtension shouldn't have the '.'.
int PmmSdAllocation::allocateFilePart(char dirFullRelativePath[], const char filenameExtension[], uint8_t filePart, uint16_t kibibytesToAllocate, uint32_t* beginBlock, uint32_t* endBlock)
{
    char filename[PMM_SD_FILENAME_MAX_LENGTH];

    // 1) How will be called the new part file?
    getFilePartName(filename, dirFullRelativePath, filePart, filenameExtension);

    // Create directory if missing
    if (!mSdFat->exists(dirFullRelativePath))
    {
        mSdFat->mkdir(dirFullRelativePath);
    }

    // 2) The SafeLog will save its backups blocks on the next file part when the Last Data Block is in the 2 last addresses of the current part,
    //    so we need to check if the filepart already exists.
    if(mSdFat->exists(filename))
    {
        getFileRange(filename, beginBlock, endBlock);
    }

    else // So the file doesn't exists!
    {
        if (kibibytesToAllocate > PMM_SD_ALLOCATION_PART_MAX_KIB)
            kibibytesToAllocate = PMM_SD_ALLOCATION_PART_MAX_KIB; // Read the comments at PmmSdAllocStatus.

        // 2) Allocate the new file!
        if (!mFile.createContiguous(filename, KIBIBYTE_IN_BYTES * kibibytesToAllocate))
        {
            advPrintf("Error at createContiguous()!\n");
            return 1;
            // error("createContiguous failed");
        }

        // 3) Get the address of the blocks of the new file on the SD. [beginBlock, endBlock].
        if (!mFile.contiguousRange(beginBlock, endBlock))
        {
            advPrintf("Error at contiguousRange()!\n");
            return 2;
            // error("contiguousRange failed");
        }

        if (!mSdioCard->erase(*beginBlock, *endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
        {
            advPrintf("Error at erase()!\n");
            return 3;
            // error("erase failed");
        }

    }

    mFile.close();

    return 0;
}