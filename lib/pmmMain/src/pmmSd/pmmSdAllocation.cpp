/* pmmSdAllocation.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <string.h>                 // For snprintf
#include "pmmSd/pmmSdAllocation.h"



PmmSdAllocation::PmmSdAllocation(SdFatSdio* sdFat)
{
    mSdFat = sdFat;
    mSdioCard = sdFat->card();
}



int PmmSdAllocation::nextBlockAndAllocIfNeeded(char dirFullRelativePath[], const char filenameExtension[], pmmSdAllocationStatusStructType* statusStruct)
{

    // 1) Do we need a new part? No!
    if (statusStruct->freeBlocksAfterCurrent)
    {
        statusStruct->freeBlocksAfterCurrent--; // We don't need a new part!
        statusStruct->currentBlock++;
        return 0;
    }
    
    // 2) YA
    // NOTE this function does currentPositionInBlock = 0; freeBlocksAfterCurrent = endBlock - statusStruct->currentBlock; nextFilePart++
    else
        return allocateFilePart(dirFullRelativePath, filenameExtension, statusStruct);
}



// Handles our pmmSdAllocationStatusStructType struct automatically.
int PmmSdAllocation::allocateFilePart(char dirFullRelativePath[], const char filenameExtension[], pmmSdAllocationStatusStructType* statusStruct)
{
    uint32_t endBlock;
    int returnValue = allocateFilePart(dirFullRelativePath, filenameExtension, statusStruct->nextFilePart, statusStruct->KiBPerPart, &(statusStruct->currentBlock), &endBlock);
    
    statusStruct->currentPositionInBlock = 0;
    statusStruct->freeBlocksAfterCurrent = endBlock - statusStruct->currentBlock;
    statusStruct->nextFilePart++;

    return returnValue;
}



// Allocates a file part with a length of X blocks.
// If no problems found, return 0.
// The filenameExtension shouldn't have the '.'.
int PmmSdAllocation::allocateFilePart(char dirFullRelativePath[], const char filenameExtension[], uint8_t filePart, uint16_t kibibytesToAllocate, uint32_t* beginBlock, uint32_t* endBlock)
{
    if (kibibytesToAllocate > PMM_SD_ALLOCATION_PART_KIB)
        kibibytesToAllocate = PMM_SD_ALLOCATION_PART_KIB; // Read the comments at pmmSdAllocationStatusStructType.

    // 1) How will be called the new part file?
    snprintf(mTempFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s/part%u.%s", dirFullRelativePath, filePart, filenameExtension);


    // 2) Allocate the new file!
    if (!mAllocationFile.createContiguous(mTempFilename, KIBIBYTE_IN_BYTES * kibibytesToAllocate))
    {
        PMM_DEBUG_ADV_PRINTLN("Error at createContiguous() (@ New File)!");
        return 1;
        // error("createContiguous failed");
    }

    // 3) Get the address of the blocks of the new file on the SD. [beginBlock, endBlock].
    if (!mAllocationFile.contiguousRange(beginBlock, endBlock))
    {
        PMM_DEBUG_PRINTLN("PmmSd: ERROR 5 - Error at contiguousRange()!");
        return 1;
        // error("contiguousRange failed");
    }

    if (!mSdioCard->erase(*beginBlock, *endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
    {
        PMM_DEBUG_PRINTLN("PmmSd: ERROR 6 - Error at erase()!");
        return 1;
        // error("erase failed");
    }
    PMM_SD_DEBUG_PRINT_MORE("PmmSd: [M] File allocation succeeded.");

    return 0;
}