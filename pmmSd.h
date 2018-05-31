#ifndef PMM_SD_h
#define PMM_SD_h

#include <Arduino.h>
#include <SdFat.h>

#define FILENAME_MAX_LENGTH 64

class SdManager
{
private:
    void yield(); // Not really sure if it is needed.
    SdFatSdioEX mSdEx;
    File mFile;
    char mFilename[FILENAME_MAX_LENGTH];

public:
    SdManager();
    int init();
    void setFilename(char *filename);
    int setFilenameAutoId(const char* baseName, const char* suffix);
    int writeToFilename(char *filename, char *arrayToWrite, int32_t length);
    int writeStringToFilename(char *filename, char *arrayToWrite);
    int writeToFile(char *arrayToWrite, int32_t length);
    int writeToFile(char *arrayToWrite);
    bool sdBusy();
    void getFilename(char *stringToReturn, uint32_t bufferLength);
};

#endif
