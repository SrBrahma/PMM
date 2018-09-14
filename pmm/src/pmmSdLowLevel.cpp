/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmConsts.h>

#include <pmmSd.h>
#include <pmmErrorsCentral.h>
#include <SdFat.h>



PmmSdFileLogPreAllocatedInParts::PmmSdFileLogPreAllocatedInParts(char* baseFilename, uint8_t sourceAddress, uint16_t blocksAllocationPerPart, uint16_t bufferSizeInBlocks)
{
    strncpy(mBaseFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s", baseFilename);
    mSourceAddress           = sourceAddress;
    mBlocksAllocationPerPart = blocksAllocationPerPart;
    mBufferSizeInBlocks      = bufferSizeInBlocks;
    
    mBufferPointer           = malloc(mBufferSizeInBlocks * 512);
    mCurrentNumberOfParts    = 0;
}



int PmmSd::allocateFilePart(pmmSdFilePartsStructType* pmmSdFilePartsStruct)
{
    static char tempFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];
    uint32_t bgnBlock, endBlock;

    // 1) How will be called the new part file?
    snprintf(tempFilename, PMM_SD_FILENAME_INTERNAL_MAX_LENGTH, "%s_%02u%s", pmmSdFilePartsStruct->baseFilename,
             pmmSdFilePartsStruct->currentNumberOfParts, pmmSdFilePartsStruct->filenameExtension);

    // 2) Allocate the new file!
    if (!pmmSdFilePartsStruct->file.createContiguous(tempFilename, PMM_SD_BLOCK_SIZE * 2 * pmmSdFilePartsStruct->kibibyteAllocationPerPart))
    {
        return 1;
        // error("createContiguous failed");

    }

    // 3) Get the address of the blocks of the new file on the SD.
    if (!pmmSdFilePartsStruct->file.contiguousRange(&bgnBlock, &endBlock))
    {
        return 1;
        // error("contiguousRange failed");
    }

    if (!mSdEx.card()->erase(bgnBlock, endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
    {
        return 1;
        // error("erase failed");
    }

    pmmSdFilePartsStruct->currentNumberOfParts++;

    return 0;
}



int PmmSd::writeInPmmFormat(pmmSdFilePartsStructType* pmmSdFilePartsStruct, uint8_t sourceAddress, uint8_t data[], uint16_t dataLength)
{
    /*
    // Start a multiple block write.
    if (!mSdEx.card()->writeStart(pmmSdFilePartsStruct->file.firstBlock()))
        return 1;
    //error("writeStart failed");


    else if (!sd.card()->isBusy())
    {
 
        if (!sd.card()->writeData((uint8_t*)pBlock))
        return 1;


    }
    if (!sd.card()->writeStop())
        return 1;
    */
   return 0;
}



int PmmSd::writeSmartSizeInPmmFormat(pmmSdFilePartsStructType* pmmSdFilePartsStruct, uint8_t sourceAddress, uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize)
{
    return 0;
}


//if (!file.open("dir2/DIR3/NAME3.txt", O_WRITE | O_APPEND | O_CREAT)) {
 // error("dir2/DIR3/NAME3.txt");

bool PmmSd::getSdIsBusy()
{
    return mSdEx.card()->isBusy();
}



char* PmmSd::getThisSessionNamePtr()
{
    return mThisSessionName;
}
