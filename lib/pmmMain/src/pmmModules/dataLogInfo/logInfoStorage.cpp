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

#define LOG_INFO_STORAGE_INDEX_TOTAL_PACKETS    0
#define LOG_INFO_STORAGE_INDEX_POSITIONS_START  1

#include <byteSelection.h>
#include "pmmModules/dataLog/dataLog.h"

// Note that it has a maximum file size of 64KiB. However, it can be easily upgraded by changing this code, but no use right now.
int PmmModuleDataLog::savePart(char filePath[], uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, int* finishedBuilding)
{
    if (*finishedBuilding)      // Reset the variable if it was true.
        *finishedBuilding = 0;

    if (!filePath)
        return 1;
    if (!data)
        return 2;

    uint8_t posDataLengthContent[2];

    mPmmSd->open(filePath);
    
// 1) If the file is new, build it!
    if (mPmmSd->fileSize() == 0)
    {
        // 1.1) Write the first field, which is the total number of parts.
        mPmmSd->write(totalParts);

        // 1.2) Write the second field, which is the positions of each part. For now, they will all be zeroes.
        for (unsigned counter = 0; counter < totalParts * 2; counter++) // * 2 as they are 2 bytes each
            mPmmSd->write(0);
    } // Just this.

// 2) We are ready for writing the received packet. Not really. First, we check if we already received this packet.
    mPmmSd->seek(LOG_INFO_STORAGE_INDEX_POSITIONS_START + (currentPart * 2));
    mPmmSd->read(posDataLengthContent, 2);
    if (posDataLengthContent[0] || posDataLengthContent[1])
        return 0;   // We already own this packet!

// 3) So we don't own this packet. Write it! First, the Data Part X Length.
    uint16_t dataPartLengthPosition = mPmmSd->fileSize(); // This will also be used later! fileSize() return an uint32_t, so, remember it if planing a > 64KiB file.
    mPmmSd->seek(dataPartLengthPosition); // Jump to the end of the file!
    mPmmSd->write(LSB0(dataLength));
    mPmmSd->write(LSB1(dataLength));

// 4) Now, write the data.
    mPmmSd->write(data, dataLength);

// 5) Now... write the Data Part X Length position in the Positions table.
    mPmmSd->seek(LOG_INFO_STORAGE_INDEX_POSITIONS_START + (currentPart * 2));
    mPmmSd->write(LSB0(dataPartLengthPosition));
    mPmmSd->write(LSB1(dataPartLengthPosition));

// 6) Before ending, we check if we finished building the package. We do it here to avoid having another function
//      which will have to reopen the file.

    mPmmSd->seek(LOG_INFO_STORAGE_INDEX_POSITIONS_START);
    for (uint8_t counter = 0; counter < totalParts; counter++)
    {
        mPmmSd->read(posDataLengthContent, 2);
        if (!(posDataLengthContent[0] | posDataLengthContent[1])) // Check for an empty field.
        {
            mPmmSd->close(); // Empty field found! Close and quit!
            return 0;
        }
    }

    *finishedBuilding = 1;
    mPmmSd->close();

    return 0;
}

int PmmModuleDataLog::saveOwnDataLogInfo()
{
    char filename[PMM_SD_FILENAME_MAX_LENGTH];
    char filenameTemp[PMM_SD_FILENAME_MAX_LENGTH];

    getDataLogDirectory(filenameTemp, PMM_SD_FILENAME_MAX_LENGTH, mDataLogId, mDataLogSize, LOG_INFO_FILENAME);
    mPmmSd->getSelfDirectory(filename, PMM_SD_FILENAME_MAX_LENGTH, filenameTemp);

    if (!mPmmSd->exists(filename))
    {
        mPmmSd->open(filename);
        mPmmSd->write(mLogInfoContentArray, mLogInfoContentArrayLength);
    }
    return 0;
}

int PmmModuleDataLog::saveReceivedDataLogInfo(uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, uint8_t dataLogId, uint8_t sourceAddress, uint8_t sourceSession)
{
    int  finishedBuilding;
    char filename[PMM_SD_FILENAME_MAX_LENGTH];
    char filenameTemp[PMM_SD_FILENAME_MAX_LENGTH];

    getDataLogDirectory(filenameTemp, PMM_SD_FILENAME_MAX_LENGTH, mDataLogId, mDataLogSize, LOG_INFO_FILENAME);
    mPmmSd->getReceivedDirectory(filename, PMM_SD_FILENAME_MAX_LENGTH, sourceAddress, sourceSession, filenameTemp);

    savePart(filename, mLogInfoContentArray, mLogInfoContentArrayLength, currentPart, totalParts, &finishedBuilding);
    if (finishedBuilding)
    {
        // Finished building the package
    }
    return 0;
}
