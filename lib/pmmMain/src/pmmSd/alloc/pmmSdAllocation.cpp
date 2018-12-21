/* pmmSdAllocation.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <string.h>                 // For snprintf
#include "pmmConsts.h"              // For Debug prints
#include "pmmSd/pmmSdConsts.h"
#include "pmmSd/alloc/pmmSdAllocation.h"


PmmSdAllocation::PmmSdAllocation(SdFatSdio* sdFat)
{
    mSdFat = sdFat;
    mSdioCard = sdFat->card();
}


void PmmSdAllocation::getFilePartName(char destinationArray[], char dirFullRelativePath[], uint8_t filePart, const char filenameExtension[])
{
    snprintf(destinationArray, PMM_SD_FILENAME_MAX_LENGTH, "%s/part-%u.%s", dirFullRelativePath, filePart, filenameExtension);
}

// Returns by reference the number of actual file parts.
// Returns 0 if no error.
int PmmSdAllocation::getNumberFileParts(char dirFullRelativePath[], const char filenameExtension[], uint8_t* fileParts)
{
    *fileParts = 0;

    if (!dirFullRelativePath || !filenameExtension || !fileParts) // invalid addresses
        return 1;


    while (1)
    {
        getFilePartName(mTempFilename, dirFullRelativePath, *fileParts, filenameExtension);
        if (mSdFat->exists(mTempFilename))
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

    if (!(returnValue = allocateFilePart(dirFullRelativePath, filenameExtension, allocStatus->nextFilePart, allocStatus->KiBPerPart, &(allocStatus->currentBlock), &endBlock)))
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

    // 1) How will be called the new part file?
    getFilePartName(mTempFilename, dirFullRelativePath, filePart, filenameExtension);

    // Create directory if missing
    if (!mSdFat->exists(dirFullRelativePath))
    {
        mSdFat->mkdir(dirFullRelativePath);  // This function is returning an error even when suceeds.
        //{
            //PMM_DEBUG_ADV_PRINTLN("Error at mkdir()!");
            //return 1;
        //}
    }

    // 2) The SafeLog will save its backups blocks on the next file part when the Last Data Block is in the 2 last addresses of the current part,
    //    so we need to check if the filepart already exists.
    if(mSdFat->exists(mTempFilename))
    {
        getFileRange(mTempFilename, beginBlock, endBlock);
    }

    else // So the file doesn't exists!
    {

        if (kibibytesToAllocate > PMM_SD_ALLOCATION_PART_MAX_KIB)
            kibibytesToAllocate = PMM_SD_ALLOCATION_PART_MAX_KIB; // Read the comments at PmmSdAllocStatus.

        // 2) Allocate the new file!
        if (!mFile.createContiguous(mTempFilename, KIBIBYTE_IN_BYTES * kibibytesToAllocate))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at createContiguous()!");
            return 1;
            // error("createContiguous failed");
        }

        // 3) Get the address of the blocks of the new file on the SD. [beginBlock, endBlock].
        if (!mFile.contiguousRange(beginBlock, endBlock))
        {
            PMM_DEBUG_ADV_PRINTLN("Error at contiguousRange()!");
            return 2;
            // error("contiguousRange failed");
        }

        if (!mSdioCard->erase(*beginBlock, *endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
        {
            PMM_DEBUG_ADV_PRINTLN("Error at erase()!");
            return 3;
            // error("erase failed");
        }

    }
    
    mFile.close();

    return 0;
}