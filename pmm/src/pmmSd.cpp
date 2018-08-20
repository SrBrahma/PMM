/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmConsts.h>

#include <pmmSd.h>
#include <pmmErrorsCentral.h>
#include <SdFat.h>
/*
// Replace "weak" system yield() function.
void PmmSd::yield()
{
    // Only count cardBusy time.
    if (!sdBusy())
    {
    return;
    }
    //uint32_t m = micros();
    //yieldCalls++;
    while (sdBusy()) {
    // Do something here.
    }
    //m = micros() - m;
    //if (m > yieldMaxUsec) {
    //yieldMaxUsec = m;
    //}
    //yieldMicros += m;
}
*/
PmmSd::PmmSd()
{
}

int PmmSd::init(PmmErrorsCentral* pmmErrorsCentral)
{
    mPmmErrorsCentral = pmmErrorsCentral;
        if (!mSdEx.begin())
    {
        PMM_DEBUG_PRINT("PmmSd #1: SD init failed!");
        mPmmErrorsCentral->reportErrorByCode(ERROR_SD);
        return 1;
    }

    mFileId = setFilenameAutoId(PMM_SD_FILENAME_BASE_PREFIX, PMM_SD_FILENAME_BASE_SUFFIX);
    #if PMM_DEBUG_SERIAL
        char tempFilename[PMM_SD_FILENAME_MAX_LENGTH];
        getFilename(tempFilename, PMM_SD_FILENAME_MAX_LENGTH);
        Serial.print("Filename is = \""); Serial.print(tempFilename); Serial.println("\"");
    #endif

    // make sdEx the current volume.
    mSdEx.chvol();
    PMM_DEBUG_PRINT_MORE("PmmSd: Initialized successfully!");
    return 0;
}
/* wt was that
    if (sdIsWorking) // This conditional exists so you can disable sd writing by changing the initial sdIsWorking value on the variable declaration.
    {
        if (mPmmSd.writeToFile(SD_LOG_HEADER, strlen(SD_LOG_HEADER)))
        {
            PMM_DEBUG_PRINT("sdIsWorking = False");
            sdIsWorking = 0;
            pmmErrorsCentral.reportErrorByCode(ERROR_SD);
        }
        else
        {
            PMM_DEBUG_PRINT("sdIsWorking = True");
        }
    }
    //END of Setup Modulo SD--------------------------------//

}*/

void PmmSd::setFilename(char *sourceFilename)
{
    snprintf(mFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s", sourceFilename);
}

int PmmSd::setFilenameAutoId(const char* baseName, const char* suffix)
{
    int fileID = 0;
    while (true)
    {
        snprintf(mFilename, PMM_SD_FILENAME_MAX_LENGTH, "%s%03u%s", baseName, fileID, suffix); // %03u to make the file id at least 3 digits.
        if (mSdEx.exists(mFilename))
            fileID++;
        else
        {
            return fileID;

        }
    }
}

int PmmSd::writeToFile(char *arrayToWrite, int32_t length)
{
    if (!mFile.open(mFilename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }
    /*
    if ((int)nb != file.read(buf, nb))
    {
        errorHalt("read failed");
    }
    // crude check of data.
    if (buf32[0] != n || buf32[nb/4 - 1] != n)
    {
        errorHalt("data check");
    }*/
    mFile.close();
    return 0;
}

int PmmSd::writeToFilename(char *filename, char *arrayToWrite, int32_t length)
{
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }
    mFile.close();
    return 0;
}

int PmmSd::writeStringToFilename(char *filename, char *arrayToWrite)
{
    int32_t length;
    if (!mFile.open(filename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    length = strlen(arrayToWrite);
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}
int PmmSd::writeToFile(char *arrayToWrite)
{
    int32_t length;
    if (!mFile.open(mFilename, O_RDWR | O_CREAT | O_APPEND)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        mFile.close();
        return 1;
    }

    length = strlen(arrayToWrite);
    if (length != mFile.write(arrayToWrite, length))
    {
        mFile.close();
        return 2;
    }

    mFile.close();
    return 0;
}

bool PmmSd::sdBusy()
{
    return mSdEx.card()->isBusy();
}

void PmmSd::getFilename(char *stringToReturn, uint32_t bufferLength)
{
    snprintf(stringToReturn, bufferLength, "%s", mFilename);
}


unsigned PmmSd::getFileId()
{
    return mFileId;
}
