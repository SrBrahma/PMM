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
#define PMM_SD_FILENAME_SUFFIX_LENGTH       6   // The extension! For example, ".txt"

#define PMM_SD_FILENAME_INTERNAL_MAX_LENGTH (PMM_SD_FILENAME_MAX_LENGTH + PMM_SD_FILENAME_SUFFIX_LENGTH + 5)

#define PMM_SD_BLOCK_SIZE                   512
#define PMM_SD_MAXIMUM_BUFFER_LENTH_KIB     16

#define PMM_SD_PLOG_MAGIC_NUMBER            'M'



uint16_t kibibytesToBlocksAmount(uint16_t kibibytes); // Kibibyte is 1024 bytes! (kilobyte is 1000 bytes!) https://en.wikipedia.org/wiki/Kibibyte
uint16_t mebibytesToBlocksAmount(uint16_t mebibytes); // Mebibyte is 1024 kibibytes! (megabyte is 1000 kilobytes!) https://en.wikipedia.org/wiki/Mebibyte



// System made to work with data of persistent length. Later I will explain it more.
// This need a deconstructor!
// The maximum buffer length is defined by PMM_SD_MAXIMUM_BUFFER_LENTH_KIB! If the given value is greater than this, will be replaced by this maximum!
// As the blocksAllocationPerPart is an uint16_t, the maximum file part size is 32MiB. As making bigger file parts doesn't seem too reasonable for the system specifications, will leave this way.
class PmmSdFileLogPreAllocatedInParts
{

public:

    PmmSdFileLogPreAllocatedInParts(SdFatSdioEX* sdEx, char* baseFilename, uint8_t sourceAddress, uint16_t blocksAllocationPerPart, uint8_t bufferSizeInBlocks, uint16_t dataLength);
    int writeSmartSizeInPmmFormat(uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables);

private:

    int allocateFilePart();
    int pmmFlush();

    // File informations
    char        mFilenameExtension[PMM_SD_FILENAME_SUFFIX_LENGTH] = ".plog";    // Must include the '.' (dot)! If no extensions is given, the file won't have one!
    char        mBaseFilename[PMM_SD_FILENAME_MAX_LENGTH];               // The code will include the current part to its name, so "name" will be "name_00", for example.
    uint8_t     mSourceAddress;

    // Blocks and Parts!
    uint16_t    mBlocksAllocationPerPart;       // How many blocks of 512 bytes the file will have? A 1MiB file have (512 * 2 * 1024) = 
    uint8_t     mCurrentNumberOfParts;
    uint32_t    mActualBlockToWrite;

    // Buffer!
    uint8_t*    mBufferPointer; // The array is malloc()'ed at the object initializer. This pointer points to the malloc()'ed array!
    
    uint16_t    mBufferSizeInBlocks;            // A buffer of this*512 bytes will be created. Bigger buffer may not necessarily means a faster write rate. Test it.

    uint16_t    mBufferTotalLength; // Being uint16_t, limits the buffer to have a maximum length of 65536; 64KiB. We will NEVER use it. (in 30 years, this comment will be funny,
    uint16_t    mBufferActualIndex; // because someone will use a 64KiB as buffer to an SD card in a microcontroller lol)

    uint16_t    mDataLength;

    // Tech stuff!
    SdFatSdioEX* mSdEx;                         // For erasing the blocks!
    File        mFile;
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

    SdFatSdioEX getSdEx();


private:
    PmmErrorsCentral *mPmmErrorsCentral;

    SdFatSdioEX mSdEx;
    File mFile;

    uint16_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string
};

#endif
