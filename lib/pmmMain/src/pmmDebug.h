#ifndef PMM_DEBUG_h
#define PMM_DEBUG_h

#include "pmmConsts.h"  // For PMM_DEBUG define

// For 
    #define STR(value)            #value                // https://stackoverflow.com/a/5966717/10247962
    #define STR2(value)           STR(value)            // https://stackoverflow.com/a/3030312/10247962
    #define DO_PRAGMA(arguments)  _Pragma(#arguments)
    #define REMINDER(string)      DO_PRAGMA(message(string))

    #define PMM_CANCEL_MACRO() do {} while (0)

    // Get only the filename from a __FILE__
    #define FILENAME (__builtin_strrchr("/" __FILE__, '/') + 1)


#if PMM_DEBUG
    #define PMM_DEBUG_ADV_PRINT(string) { Serial.print(FILENAME); Serial.print(" ("); \
        Serial.print(__func__); Serial.print(", ln " STR2(__LINE__) ") : "); Serial.print(string); }
    #define PMM_DEBUG_ADV_PRINTLN(string) { PMM_DEBUG_ADV_PRINT(string) Serial.println(); } // https://stackoverflow.com/a/8488201/10247962
    #define PMM_DEBUG_PRINT(string)   Serial.print(string)  ;
    #define PMM_DEBUG_PRINTLN(string) Serial.println(string);
#else
    #define PMM_DEBUG_ADV_PRINT(x)      PMM_CANCEL_MACRO()
    #define PMM_DEBUG_ADV_PRINTLN(x)    PMM_CANCEL_MACRO()
    #define PMM_DEBUG_PRINT(x)          PMM_CANCEL_MACRO()
    #define PMM_DEBUG_PRINTLN(x)        PMM_CANCEL_MACRO()
#endif


#if PMM_DEBUG_MORE
    #define PMM_DEBUG_MORE_TAG_PRINT()  Serial.print("[M] ");
    #define PMM_DEBUG_MORE_PRINT(x)     PMM_DEBUG_MORE_TAG_PRINT() Serial.print(x);
    #define PMM_DEBUG_MORE_PRINTLN(x)   PMM_DEBUG_MORE_PRINT(x) Serial.println();
#else
    #define PMM_DEBUG_MORE_TAG_PRINT()  PMM_CANCEL_MACRO()
    #define PMM_DEBUG_MORE_PRINT(x)     PMM_CANCEL_MACRO()
    #define PMM_DEBUG_MORE_PRINTLN(x)   PMM_CANCEL_MACRO()
#endif


#define PMM_DEBUG_PRINT_HEX_MAX_BYTES_PER_LINE      49





// IMU
#define PMM_IMU_DEBUG_MORE              0  // Prints additional debug messages.

#if PMM_IMU_DEBUG_MORE
    #define PMM_IMU_DEBUG_PRINT_MORE(x)   PMM_DEBUG_MORE_PRINT("PmmImu: ") PMM_DEBUG_PRINT(x)
    #define PMM_IMU_DEBUG_PRINTLN_MORE(x) PMM_DEBUG_MORE_PRINT("PmmImu: ") PMM_DEBUG_PRINTLN(x)
#else
    #define PMM_IMU_DEBUG_PRINTLN_MORE(x) PMM_CANCEL_MACRO();
#endif



void printHexArray(uint8_t arrayToPrint[], unsigned arrayLength);


#endif