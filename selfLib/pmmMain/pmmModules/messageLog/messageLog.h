#ifndef PORT_STRING_h
#define PORT_STRING_h

#include <stdint.h> // for uint32_t

#include "pmmTelemetry/telemetry.h"
#include "pmmSd/sd.h"

#define PORT_MESSAGE_LOG_INDEX_LSB_CRC_PACKET   0
#define PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET   1
#define PORT_MESSAGE_LOG_INDEX_STRING_X         2
#define PORT_MESSAGE_LOG_INDEX_OF_Y     3
// Total header length is equal to...
#define PORT_MESSAGE_LOG_HEADER_LENGTH          4

#define MESSAGE_LOG_MAX_STRING_LENGTH_TELEMETRY (PMM_TLM_MAX_PAYLOAD_LENGTH - PORT_MESSAGE_LOG_HEADER_LENGTH)

#define MESSAGE_LOG_NEWLINE                     "\r\n"
#define MESSAGE_LOG_MAX_STRING_LENGTH_STORAGE   MESSAGE_LOG_MAX_STRING_LENGTH_TELEMETRY + 2 // (+2 are the 2 chars from /r/n. If you change it to linux style, also do change here.

#define MESSAGE_LOG_FILENAME                    "MessageLog.txt"
#define MESSAGE_LOG_FILENAME_BACKUP             "MessageLogBackup.txt"


class PmmModuleMessageLog
{

public:

    int  init(uint32_t* mainLoopCounterPtr, PmmTelemetry* pmmTelemetry, PmmSd* pmmSd);
    int  addString(char *string); // For now, all added strings will be saved on SD and sent by telemetry.
    int  addRawString(char *string); // For now, all added strings will be saved on SD and sent by telemetry.

    void receivedPackageString(receivedPacketAllInfoStructType* packetInfo);

    int  sendString(uint8_t destinationAddress, telemetryQueuePriorities priority, uint8_t stringId); // Here the string will be loaded from the SD.
    int  sendString(uint8_t destinationAddress, telemetryQueuePriorities priority, uint8_t stringId, char string[]);

    int  loadSelfString(char stringDestination[], uint16_t requestedStringId);
    int  loadReceivedString(uint8_t sourceAddress, uint8_t sourceSession, char stringDestination[], uint16_t requestedStringId);

    int  saveSelfString(char string[]);
    int  saveReceivedString(uint8_t sourceAddress, uint8_t sourceSession, char string[]);

    uint8_t getActualNumberOfStrings();



private:
    PmmTelemetry* mPmmTelemetryPtr;
    PmmSd*        mPmmSdPtr;

    char      mSelfDirPath[PMM_SD_FILENAME_MAX_LENGTH];

    uint32_t* mMainLoopCounterPtr;

    uint8_t   mStringTransmissionCounter; // Allows buffering of the transmission.
    uint8_t   mCurrentlNumberOfStrings;


};

#endif
