/* pmmSd.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmConsts.h>

#include <pmmSd.h>
#include <pmmErrorsCentral.h>
#include <SdFat.h>



PmmSd::PmmSd()
{
}

int PmmSd::init(PmmErrorsCentral* pmmErrorsCentral)
{
    mPmmErrorsCentral = pmmErrorsCentral;

    // 1) Initialize the SD
    if (!mSdEx.begin())
    {
        PMM_DEBUG_PRINT("PmmSd #1: SD init failed!");
        mPmmErrorsCentral->reportErrorByCode(ERROR_SD);
        return 1;
    }
    // 1.1) Make sdEx the current volume.
    mSdEx.chvol();

    // 2) Creates the directory tree.
    mSdEx.mkdir(PMM_SD_BASE_DIRECTORY);
    mSdEx.chdir(PMM_SD_BASE_DIRECTORY);

    // 2.1) Get this session Id.
    // Note: If it reaches the maximum Id (999 when I wrote this), the session Id will be the maximum Id.
    // As it will probably never happen, I made it work this way. If it is a problem on future, it's someone's else problem :)
    for (mThisSessionId = 0; mThisSessionId < PMM_SD_MAX_SESSIONS_ID || mSdEx.exists(mThisSessionName); mThisSessionId++)
        snprintf(mThisSessionName, PMM_SD_FILENAME_MAX_LENGTH, "%s_%03u", PMM_THIS_NAME_DEFINE, mThisSessionId); // %03u to make the file id at least 3 digits.

    mSdEx.mkdir(mThisSessionName);
    mSdEx.chdir(mThisSessionName);

    #if PMM_DEBUG_SERIAL
        Serial.print("This Session name is = \""); Serial.print(mThisSessionName); Serial.println("\"");
    #endif

    PMM_DEBUG_PRINT_MORE("PmmSd: Initialized successfully!");
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



int PmmSd::allocateFilePart(pmmSdFilePartsStructType* pmmSdFilePartsStruct)
{
    static char tempFilename[PMM_SD_FILENAME_INTERNAL_MAX_LENGTH];
    uint32_t bgnBlock, endBlock;

    // 1) How will be called the new part file?
    snprintf(tempFilename, PMM_SD_FILENAME_INTERNAL_MAX_LENGTH, "%s_%02u%s", pmmSdFilePartsStruct->baseFilename,
             pmmSdFilePartsStruct->currentNumberOfParts, pmmSdFilePartsStruct->filenameExtension);

    // 2) Allocate the new file!
    if (!pmmSdFilePartsStruct->file.createContiguous(tempFilename, PMM_SD_BLOCK_SIZE * 2 * pmmSdFilePartsStruct->kibibyteAllocationPerPart))
    {
        return 1;
        // error("createContiguous failed");

    }

    // 3) Get the address of the blocks of the new file on the SD.
    if (!pmmSdFilePartsStruct->file.contiguousRange(&bgnBlock, &endBlock))
    {
        return 1;
        // error("contiguousRange failed");
    }

    if (!mSdEx.card()->erase(bgnBlock, endBlock)) // The erase can be 0 or 1, deppending on the card vendor's!
    {
        return 1;
        // error("erase failed");
    }

    pmmSdFilePartsStruct->currentNumberOfParts++;

    return 0;
}



int PmmSd::writeInPmmFormat(pmmSdFilePartsStructType* pmmSdFilePartsStruct, uint8_t sourceAddress, uint8_t data[], uint16_t dataLength)
{
    //return 0;

    const uint8_t QUEUE_DIM = BUFFER_BLOCK_COUNT + 1;
    // Index of last queue location.
    const uint8_t QUEUE_LAST = QUEUE_DIM - 1;

    // Allocate extra buffer space.
    block_t block[BUFFER_BLOCK_COUNT - 1];

    block_t* curBlock = 0;

    block_t* emptyStack[BUFFER_BLOCK_COUNT];
    uint8_t emptyTop;
    uint8_t minTop;

    block_t* fullQueue[QUEUE_DIM];
    uint8_t fullHead = 0;
    uint8_t fullTail = 0;

    // Use SdFat's internal buffer.
    emptyStack[0] = (block_t*)mSdEx.vol()->cacheClear();
    if (emptyStack[0] == 0)
        return 1;
        //error("cacheClear failed");

    // Put rest of buffers on the empty stack.
    for (int i = 1; i < BUFFER_BLOCK_COUNT; i++)
        emptyStack[i] = &block[i - 1];

    emptyTop = BUFFER_BLOCK_COUNT;
    minTop = BUFFER_BLOCK_COUNT;

    // Start a multiple block write.
    if (!mSdEx.card()->writeStart(pmmSdFilePartsStruct->file.firstBlock()))
        return 1;
    //error("writeStart failed");


    Serial.print(F("FreeStack: "));
    Serial.println(FreeStack());
    Serial.println(F("Logging - type any character to stop"));
    bool closeFile = false;
    uint32_t bn = 0;
    uint32_t maxLatency = 0;
    uint32_t overrun = 0;
    uint32_t overrunTotal = 0;
    uint32_t logTime = micros();

    while(1)
    {
         // Time for next data record.
        logTime += LOG_INTERVAL_USEC;
        if (Serial.available())
            closeFile = true;
        if (closeFile)
        {
            if (curBlock != 0)
            {
                // Put buffer in full queue.
                fullQueue[fullHead] = curBlock;
                fullHead = fullHead < QUEUE_LAST ? fullHead + 1 : 0;
                curBlock = 0;
            }
        }
        else
        {
            if (curBlock == 0 && emptyTop != 0)
            {
                curBlock = emptyStack[--emptyTop];
                if (emptyTop < minTop)
                    minTop = emptyTop;

                curBlock->count = 0;
                curBlock->overrun = overrun;
                overrun = 0;
            }
            if ((int32_t)(logTime - micros()) < 0)
                //error("Rate too fast");

            int32_t delta;

            do
                delta = micros() - logTime;
            while (delta < 0);

            if (curBlock == 0)
            {
                overrun++;
                overrunTotal++;
                if (ERROR_LED_PIN >= 0)
                    digitalWrite(ERROR_LED_PIN, HIGH);

                #if ABORT_ON_OVERRUN
                    Serial.println(F("Overrun abort"));
                    break;
                #endif  // ABORT_ON_OVERRUN
            }

            else

            {

                #if USE_SHARED_SPI
                    sd.card()->spiStop();
                #endif  // USE_SHARED_SPI
                    acquireData(&curBlock->data[curBlock->count++]);

                #if USE_SHARED_SPI
                    sd.card()->spiStart();
                #endif  // USE_SHARED_SPI

                if (curBlock->count == DATA_DIM)
                {
                    fullQueue[fullHead] = curBlock;
                    fullHead = fullHead < QUEUE_LAST ? fullHead + 1 : 0;
                    curBlock = 0;
                }
            }
        }
        if (fullHead == fullTail)
        {
            // Exit loop if done.
            if (closeFile)
                break;
            else if (!sd.card()->isBusy())
            {
                // Get address of block to write.
                block_t* pBlock = fullQueue[fullTail];
                fullTail = fullTail < QUEUE_LAST ? fullTail + 1 : 0;
                // Write block to SD.
                uint32_t usec = micros();
                if (!sd.card()->writeData((uint8_t*)pBlock))
                    return 1;
                    //error("write data failed");
                usec = micros() - usec;

                if (usec > maxLatency)
                    maxLatency = usec;

                // Move block to empty queue.
                emptyStack[emptyTop++] = pBlock;
                bn++;
                if (bn == FILE_BLOCK_COUNT)
                    break; // File full so stop
            }
        }

        if (!sd.card()->writeStop())
            return 1;
            //error("writeStop failed");
        Serial.print(F("Min Free buffers: "));
        Serial.println(minTop);
        Serial.print(F("Max block write usec: "));
        Serial.println(maxLatency);
        Serial.print(F("Overruns: "));
        Serial.println(overrunTotal);
        // Truncate file if recording stopped early.
        if (bn != FILE_BLOCK_COUNT)
        {
            Serial.println(F("Truncating file"));
            if (!binFile.truncate(512L * bn))
                return 1;
                // error("Can't truncate file");

        }
    }
}



int PmmSd::writeSmartSizeInPmmFormat(pmmSdFilePartsStructType* pmmSdFilePartsStruct, uint8_t sourceAddress, uint8_t* dataArrayOfPointers[], uint8_t sizesArray[], uint8_t numberVariables, uint8_t totalByteSize)
{
    return 0;
}


//if (!file.open("dir2/DIR3/NAME3.txt", O_WRITE | O_APPEND | O_CREAT)) {
 // error("dir2/DIR3/NAME3.txt");

bool PmmSd::getSdIsBusy()
{
    return mSdEx.card()->isBusy();
}



char* PmmSd::getThisSessionNamePtr()
{
    return mThisSessionName;
}
