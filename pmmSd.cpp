#include <pmmConsts.h>
#include <SdFat.h>
#include <pmmSd.h>

//------------------- SD vars --------------------//

// Replace "weak" system yield() function.
void SdManager::yield()
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
    /*m = micros() - m;
    if (m > yieldMaxUsec) {
    yieldMaxUsec = m;
    }
    yieldMicros += m;*/
}

SdManager::SdManager() {}

int SdManager::init()
{
    if (!mSdEx.begin())
        return 1; // Didnt initialized successfully
    // make sdEx the current volume.
    mSdEx.chvol();
    return 0;
}

void SdManager::setFilename(char *sourceFilename)
{
    snprintf(mFilename, FILENAME_MAX_LENGTH, "%s", sourceFilename);
}

int SdManager::setFilenameAutoId(const char* baseName, const char* suffix)
{
    int fileID = 0;
    while (true)
    {
        snprintf(mFilename, FILENAME_MAX_LENGTH, "%s%03u%s", baseName, fileID, suffix); // %03u to make the file id at least 3 digits.
        if (mSdEx.exists(mFilename))
            fileID++;
        else
        {
            return fileID;

        }
    }
}

int SdManager::writeToFile(char *arrayToWrite, int32_t length)
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

int SdManager::writeToFilename(char *filename, char *arrayToWrite, int32_t length)
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

int SdManager::writeStringToFilename(char *filename, char *arrayToWrite)
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
int SdManager::writeToFile(char *arrayToWrite)
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

bool SdManager::sdBusy()
{
    return mSdEx.card()->isBusy();
}

void SdManager::getFilename(char *stringToReturn, uint32_t bufferLength)
{
    snprintf(stringToReturn, bufferLength, "%s", mFilename);
}
