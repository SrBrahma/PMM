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

// Returns by reference the number of actual file parts.
// Returns 0 if no error.
int PmmSdAllocation::getNumberFileParts(char dirFullRelativePath[], const char filenameExtension[], uint8_t* fileParts)
{
    unsigned actualPart = 0;

    if (!dirFullRelativePath || !filenameExtension || !fileParts) // invalid addresses
        return 1;

    do
        getFilePartName(mTempFilename, dirFullRelativePath, actualPart++, filenameExtension);
    while (mFile.exists(mTempFilename));

    *fileParts = actualPart - 1;

    return 0;
}

void PmmSdAllocation::getFilePartName(char destinationArray[], char dirFullRelativePath[], uint8_t filePart, const char filenameExtension[])
{
    snprintf(destinationArray, PMM_SD_FILENAME_MAX_LENGTH, "%s/part%u.%s", dirFullRelativePath, filePart, filenameExtension);
}




int PmmSdAllocation::getFileRange(char filePath[], uint32_t *beginBlock, uint32_t *endBlock)
{
    mFile.open(filePath, O_READ);
    mFile.contiguousRange(beginBlock, endBlock);
    mFile.close();
    return 0;
}

int PmmSdAllocation::readBlock(uint32_t blockAddress, uint8_t arrayToCopyTheContent[])
{
    mSdioCard->readBlock(blockAddress, arrayToCopyTheContent);
    return 0;
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

    // 1) How will be called the new part file?
    getFilePartName(mTempFilename, dirFullRelativePath, filePart, filenameExtension);

    // 2) The SafeLog will save its backups blocks on the next file part when the Last Data Block is in the 2 last addresses of the current part,
    //    so we need to check if the filepart already exists.
    if(!mSdFat->exists(mTempFilename))
    {

        if (kibibytesToAllocate > PMM_SD_ALLOCATION_PART_KIB)
            kibibytesToAllocate = PMM_SD_ALLOCATION_PART_KIB; // Read the comments at pmmSdAllocationStatusStructType.

        // 2) Allocate the new file!
        if (!mFile.createContiguous(mTempFilename, KIBIBYTE_IN_BYTES * kibibytesToAllocate))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at createContiguous() (@ New File)!");
            return 1;
            // error("createContiguous failed");
        }

        // 3) Get the address of the blocks of the new file on the SD. [beginBlock, endBlock].
        if (!mFile.contiguousRange(beginBlock, endBlock))
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

    }
    
    mFile.close();

    return 0;
}