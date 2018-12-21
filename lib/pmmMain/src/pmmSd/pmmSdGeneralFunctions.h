#ifndef PMM_SD_GENERAL_FUNCTIONS_h
#define PMM_SD_GENERAL_FUNCTIONS_h

#include <string.h>
#include <stdint.h>
#include <SdFat.h>

#include "pmmSd/pmmSdConsts.h"

int createDirsAndOpen(SdFatSdio* sdFat, File* file, char path[], uint8_t mode = O_RDWR | O_CREAT | O_APPEND)
{
    if (!path)
        return 1;

    if (mode & O_CREAT)
    {
        // 1) Create the path if needed.
        char* lastDirectoryPosition = strrchr(path, '/');
        char  tempFilename[PMM_SD_FILENAME_MAX_LENGTH];

        if (lastDirectoryPosition && (path != lastDirectoryPosition)) // Avoid the last '/', if it's the root.
        {
            snprintf(tempFilename, lastDirectoryPosition - path, "%s", path); // This will copy the string until the last '/', which is replaced with a '\0'.

            // Create directories if doesn't exists
            if (!sdFat->exists(tempFilename))
                sdFat->mkdir(tempFilename);
        }
    }

    if (!file->open(path, mode)) // Read and write, create path if doesnt exist. http://man7.org/linux/man-pages/man2/open.2.html
    {
        file->close();
        return 2;
    }
    return 0;
}

#endif