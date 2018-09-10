/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h


#include <pmmErrorsCentral.h>
#include <pmmConsts.h>
#include <SdFat.h>

#define PMM_SD_BASE_DIRECTORY       "/Minerva Rockets/pmm"
#define PMM_SD_MAX_SESSIONS_ID      999

#define PMM_SD_FILENAME_MAX_LENGTH          64
#define PMM_SD_FILENAME_SUFFIX_LENGTH       5   // The extension! For example, ".txt"

#define PMM_SD_FILENAME_INTERNAL_MAX_LENGTH (PMM_SD_FILENAME_MAX_LENGTH + PMM_SD_FILENAME_SUFFIX_LENGTH + 5)



typedef struct
{
    File        file;
    char        baseFilename[PMM_SD_FILENAME_MAX_LENGTH];
    char        filenameSuffix[PMM_SD_FILENAME_SUFFIX_LENGTH] = {'\0'};
    uint16_t    baseBytesAllocation;
    uint8_t     numberOfParts = 0;    // https://stackoverflow.com/a/16783513/10247962
} sdFileStructType;



class PmmSd
{
private:
    PmmErrorsCentral *mPmmErrorsCentral;

    SdFatSdioEX mSdEx;
    File mFile;

    uint16_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string

    char mTempFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];

public:
    PmmSd();
    int init(PmmErrorsCentral* pmmErrorsCentral);

    int writeToFilename(char *filename, char *arrayToWrite, int32_t length);
    int writeStringToFilename(char *filename, char *arrayToWrite);

    bool getSdIsBusy();
    char* getThisSessionNamePtr();

    int allocateFilePart(sdFileStructType* sdFileStruct);
    int writeInPmmFormat(sdFileStructType* sdFileStruct, uint8_t sourceAddress, uint8_t data[], uint16_t dataLength);
    int writeSmartSizeInPmmFormat(sdFileStructType* sdFileStruct, uint8_t sourceAddress, uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize);
};

#endif
