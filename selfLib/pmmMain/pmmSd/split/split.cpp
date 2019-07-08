//    DataLogInfo File Structure
// This system will probably be used for splitting any packet in the future, like being the default in a protocol.

// [Total Packets : 1 Byte] [Position of Part 0 Length : 2 Bytes] ... [Position of Part X Length : 2 Bytes]
// [Part 0 Length : 2 Bytes] [Part 0 Data] ... [Part X Length] [Part X Data]

//    Total Packets:
// How many packets will build the entire package?

//    Position of Part X Length
// Where in this file is located the Part X Length? We point to the Length, as the Data itself is just after (+2 Bytes) the length.
// We will have (TotalPackets) of this field. Is stored in LSB format.

//    Part X Length:
// The length of this part. LSB format.

//    Part X Data:
// The data of this part.

#include <stdint.h>
#include "byteSelection.h"
#include "pmmSd/consts.h"
#include "pmmDebug.h"

#include "pmmSd/generalFunctions.h"
#include "pmmSd/split/split.h"

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
            mFile.write((uint8_t)0);
    }

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
        if (!(dataLengthContent[0] || dataLengthContent[1])) // Check for an empty field.
        {
            mFile.close(); // Empty field found! Close and quit!
            return 0;
        }
    }

    mFile.sync(); // To save the changes to the file

    if (flags & PMM_SD_SPLIT_BUILD_FLAG)
    {
        buildFileFromSplit(filePath);   // Here is the why we needed to save the changes, we are going to use them!

        if (flags & PMM_SD_SPLIT_REMOVE_FLAG)
            mFile.remove();             // Here is another why! I could use the mSdFat->remove(filePath), but, it surely is faster to remove this way.
                                        // It actually won't change anything significantly, but also didn't cost me anything to do it. I HAD TO CHOSE, OK?
    }

    mFile.close();

    return PMM_SD_SPLIT_FINISHED_PACKAGE_RETURN;
}



// This function won't check if destination file already exists, so, check before or mess will come to Earth.
int PmmSdSplit::buildFileFromSplit(char filePath[])
{
    if (!filePath)
        return 1;

    File sourceFile, targetFile;

    char sourcePath[PMM_SD_FILENAME_MAX_LENGTH];
    snprintf(sourcePath, PMM_SD_FILENAME_MAX_LENGTH, "%s%s", filePath, PMM_SD_SPLIT_EXTENSION);

    sourceFile.open(sourcePath); // O_READ flag is default flag
    targetFile.open(filePath, O_CREAT | O_RDWR);


    // 2) Read the totalPackets
    uint8_t totalPackets;
    sourceFile.seek(PMM_SD_SPLIT_INDEX_TOTAL_PACKETS);
    sourceFile.read(&totalPackets, 1);


    // 3) Copy!
    for (uint8_t packetIndex = 0; packetIndex < totalPackets; packetIndex++)
    {
        // 3.1) Get where is located the partLength of the part x.
        uint8_t  partLengthPositionArray[2];

        sourceFile.seek(PMM_SD_SPLIT_INDEX_POSITIONS_START + packetIndex * 2);
        sourceFile.read(partLengthPositionArray, 2);

        uint16_t partLengthPosition = (partLengthPositionArray[1] << 8) | partLengthPositionArray[0]; // Ensures the LSB format.
        if (partLengthPosition == 0)
            return 2;   // This part still missing in the Split file.

        // 3.2) Get the partLength of the part x.
        uint8_t  partLengthArray[2];
        sourceFile.seek(partLengthPosition);
        sourceFile.read(partLengthArray, 2);

        uint16_t partLength = (partLengthArray[1] << 8) | partLengthArray[0];

        // 3.3) Actually copy the part to the target file.
        uint16_t bytesRemaining = partLength;
        while (bytesRemaining > 0)
        {
            uint8_t  buffer[PMM_SD_SPLIT_BUFFER_LENGTH];
            uint16_t bytesToCopyNow = ((bytesRemaining > PMM_SD_SPLIT_BUFFER_LENGTH) ? PMM_SD_SPLIT_BUFFER_LENGTH : bytesRemaining);
            sourceFile.read(buffer, bytesToCopyNow);
            targetFile.write(buffer, bytesToCopyNow);
            bytesRemaining -= bytesToCopyNow;
        }
    }

    sourceFile.close();
    targetFile.close();

    return 0;
}