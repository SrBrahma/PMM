/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h


#include <pmmErrorsCentral.h>
#include <pmmConsts.h>
#include <SdFat.h>

#define PMM_SD_BASE_DIRECTORY               "/Minerva Rockets/pmm"
#define PMM_SD_BACKUP_SUFFIX                "_backup"

#define PMM_SD_MAX_SESSIONS_ID              999

#define PMM_SD_FILENAME_MAX_LENGTH          64
#define PMM_SD_FILENAME_SUFFIX_LENGTH       5   // The extension! For example, ".txt"

#define PMM_SD_FILENAME_INTERNAL_MAX_LENGTH (PMM_SD_FILENAME_MAX_LENGTH + PMM_SD_FILENAME_SUFFIX_LENGTH + 5)

#define PMM_SD_BLOCK_SIZE                   512



typedef struct
{
    File        file;
    char        baseFilename[PMM_SD_FILENAME_MAX_LENGTH];   // The code will include the current part to its name, so "name" will be "name_00", for example.
    char        filenameExtension[PMM_SD_FILENAME_SUFFIX_LENGTH] = "";  // Must include the '.' (dot)! If no extensions is given, the file won't have one!
    uint16_t    kibibyteAllocationPerPart;                  // kibibyte is 1024 bytes! (kilobyte is 1000 bytes!) https://en.wikipedia.org/wiki/Kibibyte
    uint8_t     currentNumberOfParts = 0;                   // https://stackoverflow.com/a/16783513/10247962
    
} pmmSdFilePartsStructType;



class PmmSd
{
private:
    PmmErrorsCentral *mPmmErrorsCentral;

    SdFatSdioEX mSdEx;
    File mFile;

    uint16_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string

    int allocateFilePart(pmmSdFilePartsStructType* pmmSdFilePartsStruct);

public:
    PmmSd();
    int init(PmmErrorsCentral* pmmErrorsCentral);

    int writeToFilename(char *filename, char *arrayToWrite, int32_t length);
    int writeStringToFilename(char *filename, char *arrayToWrite);

    bool getSdIsBusy();
    char* getThisSessionNamePtr();

    int writeTextFileWithBackup(char filename[], uint8_t sourceAddress, char stringToWrite[]);

    int writeInPmmFormat(pmmSdFilePartsStructType* pmmSdFilePartsStruct, uint8_t sourceAddress, uint8_t data[], uint16_t dataLength);
    int writeSmartSizeInPmmFormat(pmmSdFilePartsStructType* pmmSdFilePartsStruct, uint8_t sourceAddress, uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize);
};

#endif
