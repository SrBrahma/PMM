/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmConsts.h>

#include <pmmSd.h>
#include <pmmErrorsCentral.h>
#include <SdFat.h>



PmmSdFileLogPreAllocatedInParts::PmmSdFileLogPreAllocatedInParts(SdFatSdioEX* sdEx, char* baseFilename, uint8_t sourceAddress, uint16_t blocksAllocationPerPart, uint8_t bufferSizeInBlocks, uint16_t dataLength)
{
    // 1) Do some basic init stuff
    mCurrentNumberOfParts    = 0;
    mBufferActualIndex       = 0;


    // 2) Get the arguments
    mSdEx = sdEx;
    snprintf(mBaseFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s", baseFilename);
    mSourceAddress           = sourceAddress;
    mBlocksAllocationPerPart = blocksAllocationPerPart;
    mBufferSizeInBlocks      = bufferSizeInBlocks;
    mDataLength              = dataLength;


    // 3) Calculate the buffer size. If the given size is greater than the maximum, replace it! Sometimes, the user DOESN'T know what he is doing!
    mBufferTotalLength       = mBufferSizeInBlocks * PMM_SD_BLOCK_SIZE;

    if ((mBufferTotalLength / 1024) > PMM_SD_MAXIMUM_BUFFER_LENTH_KIB)
        mBufferTotalLength   = PMM_SD_MAXIMUM_BUFFER_LENTH_KIB;


    // 4) malloc() the buffer. An error should raise if the malloc fails. But, NO TIME!
    mBufferPointer           = (uint8_t*) malloc(mBufferTotalLength);
    // if (!mBufferPointer) ERROR OH NO
    
}



int PmmSdFileLogPreAllocatedInParts::allocateFilePart()
{
    static char tempFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];

    uint32_t bgnBlock, endBlock;

    // 1) How will be called the new part file?
    snprintf(tempFilename, PMM_SD_FILENAME_INTERNAL_MAX_LENGTH, "%s_%02u%s", mBaseFilename,
             mCurrentNumberOfParts, mFilenameExtension);

    // 2) Allocate the new file!
    if (!mFile.createContiguous(tempFilename, PMM_SD_BLOCK_SIZE * mBlocksAllocationPerPart))
    {
        return 1;
        // error("createContiguous failed");

    }

    // 3) Get the address of the blocks of the new file on the SD.
    if (mFile.contiguousRange(&bgnBlock, &endBlock))
    {
        return 1;
        // error("contiguousRange failed");
    }

    if (!mSdEx->card()->erase(bgnBlock, endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
    {
        return 1;
        // error("erase failed");
    }

    mActualBlockToWrite = bgnBlock;
    mCurrentNumberOfParts++;

    return 0;
}

int PmmSdFileLogPreAllocatedInParts::pmmFlush()
{
    unsigned blockCounter;

    // 1) Start a multiple block write.
    if (!mSdEx->card()->writeStart(mActualBlockToWrite))
        return 1;
    //error("writeStart failed");
 
    // 2) Write the blocks!
    for (blockCounter = 0; blockCounter < mBufferSizeInBlocks; blockCounter++)
        if (!mSdEx->card()->writeData(mBufferPointer + blockCounter * PMM_SD_BLOCK_SIZE))
            return 1;

    // 3) Stop the writing!
    mActualBlockToWrite += blockCounter; // Increases the actual block by the number of blocks written, so in the next flush, we will write continuosly!
    if (!mSdEx->card()->writeStop())
        return 1;
}

int PmmSdFileLogPreAllocatedInParts::writeInPmmFormat(uint8_t data[], uint16_t dataLength)
{
    /*
    
    */
   return 0;
}



int PmmSdFileLogPreAllocatedInParts::writeSmartSizeInPmmFormat(uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables)
{
    uint8_t variableCounter;

    mBufferPointer[mBufferActualIndex++] = PMM_SD_PLOG_MAGIC_NUMBER;

    while (numberVariables--)
    {
        // This take some time to understand. I don't actually expect no one else beyond me to get it. Kidding (I wasn't going to write this at first :) ).
        // You must first understand how my SmartSize works. You have an array of pointers, and another array which tells the
        // length of each variable that the pointer points to. So, we are copying each variable of X bytes to the buffer,
        // and then we increse the arrays indexes. Looks like I ain't the big genius I tried to look like, saying you couldn't understand me.
        // But it's indeed a good system, the SmartSizes. It's 00:30 now, sorry.

        if (*sizesArray > 8)
            *siz
        memcpy(mBufferPointer + mBufferActualIndex, *dataArrayOfPointers, *sizesArray);
        
        mBufferActualIndex += *sizesArray;
        dataArrayOfPointers++;
        sizesArray++;
    }

    return 0;
}


//if (!file.open("dir2/DIR3/NAME3.txt", O_WRITE | O_APPEND | O_CREAT)) {
 // error("dir2/DIR3/NAME3.txt");

