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


uint16_t kibibytesToBlocksAmount(uint16_t kibibytes); // Kibibyte is 1024 bytes! (kilobyte is 1000 bytes!) https://en.wikipedia.org/wiki/Kibibyte
uint16_t mebibytesToBlocksAmount(uint16_t mebibytes); // Mebibyte is 1024 kibibytes! (megabyte is 1000 kilobytes!) https://en.wikipedia.org/wiki/Mebibyte



class PmmSdFileLogPreAllocatedInParts
{

public:

    PmmSdFileLogPreAllocatedInParts(SdFatSdioEX* sdEx, char* baseFilename, uint8_t sourceAddress, uint16_t blocksAllocationPerPart, uint16_t bufferSizeInBlocks);
    int writeInPmmFormat(uint8_t sourceAddress, uint8_t data[], uint16_t dataLength);
    int writeSmartSizeInPmmFormat(uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize);

private:

    int allocateFilePart();

    char        mFilenameExtension[6] = ".plog";    // Must include the '.' (dot)! If no extensions is given, the file won't have one!
    char        mBaseFilename[PMM_SD_FILENAME_MAX_LENGTH];               // The code will include the current part to its name, so "name" will be "name_00", for example.
    uint8_t     mSourceAddress;
    uint16_t    mBlocksAllocationPerPart;       // How many blocks of 512 bytes the file will have? A 1MiB file have (512 * 2 * 1024) = 
    uint16_t    mBufferSizeInBlocks;            // A buffer of this*512 bytes will be created. Bigger buffer may not necessarily means a faster write rate. Test it.
    
    File        mFile;
    uint8_t*    mBufferPointer;
    uint8_t     mCurrentNumberOfParts;
    SdFatSdioEX* mSdEx;                         // For erasing the blocks!

    

};



class PmmSd
{
public:
    PmmSd();
    
    int init(PmmErrorsCentral* pmmErrorsCentral);

    int writeToFilename(char *filename, char *arrayToWrite, int32_t length);
    int writeStringToFilename(char *filename, char *arrayToWrite);

    bool getSdIsBusy();
    char* getThisSessionNamePtr();

    int writeTextFileWithBackup(char filename[], uint8_t sourceAddress, char stringToWrite[]);


private:
    PmmErrorsCentral *mPmmErrorsCentral;

    SdFatSdioEX mSdEx;
    File mFile;

    uint16_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string
};

#endif
