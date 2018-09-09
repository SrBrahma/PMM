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

class PmmSd
{
private:
    PmmErrorsCentral *mPmmErrorsCentral;

    SdFatSdioEX mSdEx;
    File mFile;

    uint16_t mThisSessionId;
    char mThisSessionName[PMM_SD_FILENAME_MAX_LENGTH]; // The full "systemName_Id" string

public:
    PmmSd();
    int init(PmmErrorsCentral* pmmErrorsCentral);

    int writeToFilename(char *filename, char *arrayToWrite, int32_t length);
    int writeStringToFilename(char *filename, char *arrayToWrite);

    bool getSdIsBusy();
    char* getThisSessionNamePtr();

    void writeInPmmFormat(File file, uint8_t data[], uint16_t dataLength, uint8_t sourceAddress);
};

#endif
