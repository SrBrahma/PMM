#ifndef PMM_SD_h
#define PMM_SD_h

#include <Arduino.h>
#include <SdFat.h>
#include <pmmErrorsAndSignals.h>
#include <pmmConsts.h>


class PmmSd
{
private:
    void yield(); // Not really sure if it is needed.
    SdFatSdioEX mSdEx;
    File mFile;
    char mFilename[PMM_SD_FILENAME_MAX_LENGTH];
    unsigned mFileId;
    unsigned mSdIsWorking;

public:
    PmmSd();
    int init(PmmErrorsAndSignals pmmErrorsAndSignals);
    void setFilename(char *filename);
    int setFilenameAutoId(const char* baseName, const char* suffix);
    int writeToFilename(char *filename, char *arrayToWrite, int32_t length);
    int writeStringToFilename(char *filename, char *arrayToWrite);
    int writeToFile(char *arrayToWrite, int32_t length);
    int writeToFile(char *arrayToWrite);
    bool sdBusy();
    void getFilename(char *stringToReturn, uint32_t bufferLength);
    unsigned getFileId();
    unsigned getSdIsWorking();
};

#endif
