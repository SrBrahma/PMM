/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <SdFat.h>

#include "pmmConsts.h"

#include "pmmSd/pmmSd.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"

#include "pmmTelemetry/pmmTelemetryProtocols.h" // For PMM_TELEMETRY_ADDRESS_SELF define



PmmSd::PmmSd()
{
}

int PmmSd::init(PmmErrorsCentral* pmmErrorsCentral, uint8_t sessionId)
{
    mPmmErrorsCentral = pmmErrorsCentral;

    // 1) Initialize the SD
    if (!mSd.begin())
    {
        PMM_DEBUG_PRINT("PmmSd: ERROR 1 - SD init failed!");
        mPmmErrorsCentral->reportErrorByCode(ERROR_SD);
        return 1;
    }
    // 1.1) Make sdEx the current volume.
    mSd.chvol();

    // 2) Creates the directory tree.
    mSd.mkdir(PMM_SD_BASE_DIRECTORY);
    mSd.chdir(PMM_SD_BASE_DIRECTORY);



    PMM_DEBUG_PRINT_MORE("PmmSd: [M] Initialized successfully!");
    return 0;
}

// sourceAddress is from where we did receive the message, for example, the PMM_TELEMETRY_ADDRESS_SELF define.
// sourceSession is from which session we did receive the message. This field is useless if the soureAddress is the
//  PMM_TELEMETRY_ADDRESS_SELF define, so it's an optional argument.
int PmmSd::println(char filename[], char string[], uint8_t sourceAddress, uint8_t sourceSession)
{
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    if (strlen(string) + 2 != mFile.println(string)) // + 2 as it will write the normal chars from the string, plus the '\r' and the '\n' (they are added in the file.println().
    {                                                // comparing to -1 the result is problematic, as the println function returns a sum of the write(string) + write("\r\n"),
                                                     // so it's possible to return a false negative.
        PMM_DEBUG_PRINT("PmmSd: ERROR 2 - The string haven't been successfully written!");
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}

// sourceAddress is from where we did receive the message, for example, the PMM_TELEMETRY_ADDRESS_SELF define.
// sourceSession is from which session we did receive the message. This field is useless if the soureAddress is the
//  PMM_TELEMETRY_ADDRESS_SELF define, so it's an optional argument.
// length argument is int as the return value of the file.write() is also int, so when comparing the results, the same sign is used.
int PmmSd::write(char filename[], char arrayToWrite[], size_t length, uint8_t sourceAddress, uint8_t sourceSession)
{

    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    if (mFile.write(arrayToWrite, length) == -1)
    {
        PMM_DEBUG_PRINT("PmmSd: ERROR 3 - The data haven't been successfully written!");
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}



void PmmSd::getFilenameOwn(char destination[], uint8_t maxLength, char filename[])
{
    snprintf(destination, maxLength, "_self/%s", filename);
}

void PmmSd::getFilenameReceived(char destination[], uint8_t maxLength, uint8_t sourceAddress, uint8_t sourceSession, char filename[])
{
    snprintf(destination, maxLength, "%03u/%03u/%s", sourceAddress, sourceSession, filename);
}





// Allocates a file part with a length of X blocks.
// Returns the address of the first block.
// If any error found, will return 0.
// The filenameExtension shouldn't have the '.'.
uint32_t PmmSd::allocateFilePart(char dirFullRelativePath[], char filenameExtension[], uint8_t filepart, uint16_t blocksToAllocateInThisPart)
{
    static char newFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];

    uint32_t bgnBlock, endBlock;


    // 1) How will be called the new part file?
    snprintf(newFilename, PMM_SD_FILENAME_INTERNAL_MAX_LENGTH, "%s_%02u.%s", baseFilename, filePartId, filenameExtension);


    // 2) Allocate the new file!
    if (!mAllocationFile.createContiguous(newFilename, PMM_SD_BLOCK_SIZE * blocksToAllocateInThisPart))
    {
        PMM_DEBUG_PRINT("PmmSd: ERROR 4 - Error at createContiguous()");
        return 1;
        // error("createContiguous failed");
    }

    // 3) Get the address of the blocks of the new file on the SD.
    if (!mAllocationFile.contiguousRange(&bgnBlock, &endBlock))
    {
        PMM_DEBUG_PRINT("PmmSd: ERROR 5 - Error at contiguousRange()");
        return 1;
        // error("contiguousRange failed");
    }

    if (!mSd.card()->erase(bgnBlock, endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
    {
        PMM_DEBUG_PRINT("PmmSd: ERROR 6 - Error at erase()");
        return 1;
        // error("erase failed");
    }

    return bgnBlock;
}


bool PmmSd::getSdIsBusy()
{
    return mSd.card()->isBusy();
}