
#include <stdint.h>
#include "pmmSd/pmmSdConsts.h"
#include "byteSelection.h"

#include "pmmSd/pmmSdGeneralFunctions.h"
#include "pmmSd/split/pmmSdSplit.h"

PmmSdSplit::PmmSdSplit(SdFatSdio* sdFat)
{
    mSdFat = sdFat;
}


// This function returns an negative number in case of an error.
// Returns 0 if all ok.
// Returns 1 if has finished building the entire package.

// Note that it has a maximum file size of 64KiB. However, it can be easily upgraded by changing this code, but no use right now.
int PmmSdSplit::savePart(char filePath[], uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, int flags)
{

    if (!filePath)
        return -1;
    if (!data)
        return -2;

    uint8_t dataLengthContent[2];

    char newFilePath[PMM_SD_FILENAME_MAX_LENGTH];
    snprintf(newFilePath, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", filePath, PMM_SD_SPLIT_EXTENSION);

    createDirsAndOpen(mSdFat, &mFile, newFilePath);
    
// 1) If the file is new, build it!
    if (mFile.fileSize() == 0)
    {
        // 1.1) Write the first field, which is the total number of parts.
        mFile.write(totalParts);

        // 1.2) Write the second field, which is the positions of each part. For now, they will all be zeroes.
        for (unsigned counter = 0; counter < totalParts * 2; counter++) // * 2 as they are 2 bytes each
            mFile.write(0);
    } // Just this.

// 2) We are ready for writing the received packet. Not really. First, we check if we already received this packet.
    mFile.seek(PMM_SD_SPLIT_INDEX_POSITIONS_START + (currentPart * 2));
    mFile.read(dataLengthContent, 2);
    if (dataLengthContent[0] || dataLengthContent[1])
        return 0;   // We already own this packet!

// 3) So we don't own this packet. Write it! First, the Data Part X Length.
    uint16_t dataPartLengthPosition = mFile.fileSize(); // This will also be used later! fileSize() return an uint32_t, so, remember it if planing a > 64KiB file.
    mFile.seek(dataPartLengthPosition); // Jump to the end of the file!
    mFile.write(LSB0(dataLength));
    mFile.write(LSB1(dataLength));

// 4) Now, write the data.
    mFile.write(data, dataLength);

// 5) Now... write the Data Part X Length position in the Positions table.
    mFile.seek(PMM_SD_SPLIT_INDEX_POSITIONS_START + (currentPart * 2));
    mFile.write(LSB0(dataPartLengthPosition));
    mFile.write(LSB1(dataPartLengthPosition));

// 6) Before ending, we check if we finished building the package. We do it here to avoid having another function
//      which will have to reopen the file.
    mFile.seek(PMM_SD_SPLIT_INDEX_POSITIONS_START);
    for (uint8_t counter = 0; counter < totalParts; counter++)
    {
        mFile.read(dataLengthContent, 2);
        if (!(dataLengthContent[0] | dataLengthContent[1])) // Check for an empty field.
        {
            mFile.close(); // Empty field found! Close and quit!
            return 0;
        }
    }


    if (flags & PMM_SD_SPLIT_REMOVE_FLAG)
        mFile.remove();

    mFile.close();

    return PMM_SD_SPLIT_FINISHED_PACKAGE_RETURN;
}



int PmmSdSplit::buildFileFromSplit(char filePath[])
{
    File sourceFile;
    
    uint8_t  buffer[512];
    uint16_t bufferLength;

    char sourcePath[PMM_SD_FILENAME_MAX_LENGTH];
    snprintf(sourcePath, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", filePath, PMM_SD_SPLIT_EXTENSION);
    
    sourceFile.open(sourcePath); // O_READ flag is default flag
    createDirsAndOpen(mSdFat, &mFile, filePath);
    
    return 0;
}