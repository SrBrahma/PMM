#ifndef PMM_SD_GENERAL_FUNCTIONS_h
#define PMM_SD_GENERAL_FUNCTIONS_h

#include <stdint.h>
#include <SdFat.h>

int createDirsAndOpen(SdFatSdio* sdFat, File* file, char path[], uint8_t mode = O_RDWR | O_CREAT);

#endif