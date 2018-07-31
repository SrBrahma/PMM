/* pmmSd.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_SD_h
#define PMM_SD_h

#include <Arduino.h>

#include <pmmErrorsCentral.h>
#include <pmmConsts.h>
#include <SdFat.h>

class PmmSd
{
private:
    //void yield(); // Not really sure if it is needed.
    SdFatSdioEX mSdEx;
    File mFile;
    char mFilename[PMM_SD_FILENAME_MAX_LENGTH];
    uint16_t mFileId;
    PmmErrorsCentral *mPmmErrorsCentral;

public:
    PmmSd();
    int init(PmmErrorsCentral* pmmErrorsCentral);
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
