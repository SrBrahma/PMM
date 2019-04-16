#ifndef PMM_DEBUG_h
#define PMM_DEBUG_h



#include "pmmConsts.h"  // For PMM_DEBUG define


#define PMM_DEBUG_PRINT_HEX_MAX_BYTES_PER_LINE      49
void    printArrayHex(uint8_t arrayToPrint[], unsigned arrayLength);


#define PMM_CANCEL_MACRO() { do {} while (0); }


#define STR(value)            #value                // https://stackoverflow.com/a/5966717/10247962
#define STR2(value)           STR(value)
#define DO_PRAGMA(arguments)  _Pragma(#arguments)
#define REMINDER(string)      DO_PRAGMA(message(string))


// Get only the filename from a __FILE__. (Currently disabled)
// Getting the FILENAME: https://stackoverflow.com/a/8488201/10247962
#define FILENAME (__builtin_strrchr("/" __FILE__, '/') + 1)


#if PMM_DEBUG
    #define PMM_DEBUG_PRINT(a)          Serial.print(a);
    #define PMM_DEBUG_PRINTLN(a)        Serial.println(a);
    #define PMM_DEBUG_PRINTF(...)       Serial.printf(__VA_ARGS__);
#else
    #define PMM_DEBUG_PRINT(x)          PMM_CANCEL_MACRO()
    #define PMM_DEBUG_PRINTLN(x)        PMM_CANCEL_MACRO()
    #define PMM_DEBUG_PRINTF(...)       PMM_CANCEL_MACRO()
#endif



#define advOnlyPrint()    { PMM_DEBUG_PRINT(FILENAME) PMM_DEBUG_PRINT(" (") PMM_DEBUG_PRINT(__func__) PMM_DEBUG_PRINT(", ln " STR2(__LINE__) "): ") }
#define advOnlyPrintln()  { advOnlyPrint() PMM_DEBUG_PRINTLN("") }
#define advPrintf(...)    { advOnlyPrint() PMM_DEBUG_PRINTF(__VA_ARGS__)  }



// =============
// Debug More
// =============

#define PMM_DEBUG_MORE_TAG              "[M] "

// PMM
#define PMM_DEBUG_MORE_BASE             "Pmm: "
#if PMM_DEBUG_MORE
    #define debugMorePrintf(...)  PMM_DEBUG_PRINT(PMM_DEBUG_MORE_TAG PMM_DEBUG_MORE_BASE) PMM_DEBUG_PRINTF(__VA_ARGS__)
#else
    #define debugMorePrintf(...)  PMM_CANCEL_MACRO()
#endif

// IMU
#define PMM_IMU_DEBUG_MORE_BASE         "PmmImu: "
#if PMM_IMU_DEBUG_MORE
    #define imuDebugMorePrintf(...)  PMM_DEBUG_PRINT(PMM_DEBUG_MORE_TAG PMM_IMU_DEBUG_MORE_BASE) PMM_DEBUG_PRINTF(__VA_ARGS__)
#else
    #define imuDebugMorePrintf(...)  PMM_CANCEL_MACRO();
#endif

// GPS
#define PMM_GPS_DEBUG_MORE_BASE         "PmmGps: "
#if PMM_GPS_DEBUG_MORE
    #define gpsDebugMorePrintf(...)  PMM_DEBUG_PRINT(PMM_DEBUG_MORE_TAG PMM_GPS_DEBUG_MORE_BASE) PMM_DEBUG_PRINTF(__VA_ARGS__)
#else
    #define gpsDebugMorePrintf(...)  PMM_CANCEL_MACRO();
#endif

// SD
#define PMM_SD_DEBUG_MORE_BASE          "PmmSd: "
#if PMM_SD_DEBUG_MORE
    #define sdDebugMorePrintf(...)  PMM_DEBUG_PRINT(PMM_DEBUG_MORE_TAG PMM_SD_DEBUG_MORE_BASE) PMM_DEBUG_PRINTF(__VA_ARGS__)
#else
    #define sdDebugMorePrintf(...)  PMM_CANCEL_MACRO();
#endif

// Telemetry
#define PMM_TLM_DEBUG_MORE_BASE         "PmmTelemetry: "
#if PMM_TLM_DEBUG_MORE
    #define tlmDebugMorePrintf(...)  PMM_DEBUG_PRINT(PMM_DEBUG_MORE_TAG PMM_TLM_DEBUG_MORE_BASE) PMM_DEBUG_PRINTF(__VA_ARGS__)
#else
    #define tlmDebugMorePrintf(...)  PMM_CANCEL_MACRO();
#endif



#endif