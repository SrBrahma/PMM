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

#ifndef PMM_SD_SPLIT_h
#define PMM_SD_SPLIT_h

#include <stdint.h>
#include <SdFat.h>

#define PMM_SD_SPLIT_BUFFER_LENGTH              512     // This value won't change too much, so don't try to improve overall performance in here.

#define PMM_SD_SPLIT_INDEX_TOTAL_PACKETS        0
#define PMM_SD_SPLIT_INDEX_POSITIONS_START      1

#define PMM_SD_SPLIT_EXTENSION                  ".splt"

#define PMM_SD_SPLIT_BUILD_FLAG                 1   // Will build the complete file when all parts are received.
#define PMM_SD_SPLIT_REMOVE_FLAG                2   // Will remove the .splt file after receiving all parts.

#define PMM_SD_SPLIT_FINISHED_PACKAGE_RETURN    1       

class PmmSdSplit
{

public:

    PmmSdSplit(SdFatSdio* sdFat);

    int savePart(char filePath[], uint8_t data[], uint16_t dataLength, uint8_t currentPart, uint8_t totalParts, int flags = PMM_SD_SPLIT_BUILD_FLAG | PMM_SD_SPLIT_REMOVE_FLAG);
    int buildFileFromSplit(char filePath[]);

private:
    SdFatSdio* mSdFat;
    File       mFile;

};

#endif