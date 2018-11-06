/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmConsts.h>

#include "pmmSd/pmmSd.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"
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
        mBufferTotalLength = PMM_SD_MAXIMUM_BUFFER_LENTH_KIB;


    // 4) malloc() the buffer. An error should raise if the malloc fails. But, NO TIME!
    mBufferPointer           = (uint8_t*) malloc(mBufferTotalLength);
    // if (!mBufferPointer) ERROR OH NO

    // 5) Allocate the first part of the file
    allocateFilePart();
    
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

    mActualBlockAddress = bgnBlock;

    mCurrentNumberOfParts++;

    mActualBlockInPart = 0; // !

    return 0;
}



int PmmSdFileLogPreAllocatedInParts::pmmFlush()
{
    unsigned alreadyWrittenBlocks = 0;
    unsigned availableBlocksOnThisPart;
    unsigned blocksToWriteNow;
    unsigned remainingBlocksToWrite = mBufferSizeInBlocks;

    // To write in multiple parts
    while(remainingBlocksToWrite)
    {   
        availableBlocksOnThisPart = mBlocksAllocationPerPart - mActualBlockInPart;
        
        if (remainingBlocksToWrite > availableBlocksOnThisPart)
            blocksToWriteNow = availableBlocksOnThisPart;

        if (remainingBlocksToWrite <= availableBlocksOnThisPart)
            blocksToWriteNow = remainingBlocksToWrite;

        // 1) Can we flush something in this file part?
        if (blocksToWriteNow > 0)
        {
            // 1.2) Write the blocks!
            if (!mSdEx->card()->writeBlocks(mActualBlockAddress,
                                            mBufferPointer + alreadyWrittenBlocks * PMM_SD_BLOCK_SIZE,
                                            blocksToWriteNow));
                return 1;

            alreadyWrittenBlocks += blocksToWriteNow;
            mActualBlockAddress += alreadyWrittenBlocks; // Increases the actual block by the number of blocks written, so in the next flush, we will write continuosly!
        }


        // 2) If we still have data to write, it's because our file part is full! Allocate a new part!
        if (remainingBlocksToWrite)
            allocateFilePart();
            // mActualBlockInPart = 0; Happens at allocateFilePart()!

    } // End of while loop

    mBufferActualIndex = 0; // !
    return 0;
}



int PmmSdFileLogPreAllocatedInParts::writeInPmmFormat(uint8_t dataArray[])
{
    unsigned availableBytesOnBuffer;
    unsigned remainingDataBytes;
    unsigned bytesToWriteNow;
    unsigned alreadyWrittenDataBytes = 0;

    // 1) Is the given array NULL?
    if (!dataArray)
        return 1;

    // 2) Write the Magic Number Start to the buffer
    // 2.1) Is there space on the buffer to write it? If not, flush to the SD and reset the actual Index!
    if (mBufferActualIndex >= mBufferTotalLength - 1)
        pmmFlush();
        // mBufferActualIndex = 0; Happens at pmmFlush()!

    // 2.2) Write it!
    mBufferPointer[mBufferActualIndex] = PMM_SD_PLOG_MAGIC_NUMBER_START;
    mBufferActualIndex++;


    // 3) Write the data to the buffer
 
    remainingDataBytes = mDataLength;

    // 3.1) Thinking on the future, if there there is a data > buffer size.
    // If the buffer size is 512 bytes, isn't too hard to have a log greater than it.
    while(remainingDataBytes)  
    {
        // 3.2) How much can we write on the buffer without flushing?
        availableBytesOnBuffer = mBufferTotalLength - mBufferActualIndex;


        // 3.3) How much data can we write, with our current available buffer?
        if (remainingDataBytes > availableBytesOnBuffer)
            bytesToWriteNow = availableBytesOnBuffer;

        if (remainingDataBytes <= availableBytesOnBuffer)
            bytesToWriteNow = remainingDataBytes;


        // 3.4) Can we write something now?
        if (bytesToWriteNow > 0)
        {
            // Yes, we can!
            memcpy(mBufferPointer + mBufferActualIndex, dataArray + alreadyWrittenDataBytes, bytesToWriteNow);
            alreadyWrittenDataBytes += bytesToWriteNow;
            remainingDataBytes      -= bytesToWriteNow;
            mBufferActualIndex      += bytesToWriteNow;
        }


        // 3.5) If we still have data to be written to the buffer, it's because the buffer is full! FLUSH!
        if (remainingDataBytes)
            pmmFlush();
            // mBufferActualIndex = 0; Happens at pmmFlush()!

    }

    // 4) Write the Magic Number End to the buffer
    // 4.1) Is there space on the buffer to write it? If not, flush to the SD and reset the actual Index!
    if (mBufferActualIndex >= mBufferTotalLength - 1)
        pmmFlush();
        // mBufferActualIndex = 0; Happens at pmmFlush()!


    // 4.2) Write it!
    mBufferPointer[mBufferActualIndex] = PMM_SD_PLOG_MAGIC_NUMBER_END;
    mBufferActualIndex++;
    return 0;
}




//if (!file.open("dir2/DIR3/NAME3.txt", O_WRITE | O_APPEND | O_CREAT)) {
 // error("dir2/DIR3/NAME3.txt");

