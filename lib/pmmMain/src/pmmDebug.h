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

/*
// https://stackoverflow.com/a/45043324/10247962
    #define CAT( A, B ) A ## B
    #define SELECT(NAME, NUM) CAT(NAME ## _, NUM)
    #define COMPOSE(NAME, ARGS) NAME ARGS

    #define GET_COUNT(_0, _1, _2, _3, _4, _5, _6, COUNT, ...) COUNT
    #define EXPAND() ,,,,,, // 6 commas (or 7 empty tokens)
    #define VA_SIZE(...) COMPOSE(GET_COUNT, (EXPAND __VA_ARGS__ (), 0, 6, 5, 4, 3, 2, 1))

    #define VA_SELECT(NAME, ...) SELECT(NAME, VA_SIZE(__VA_ARGS__))(__VA_ARGS__)
*/

#if PMM_DEBUG
    #define PMM_DEBUG_PRINT_BASE(a)         Serial.print(a);
    #define PMM_DEBUG_PRINTLN_BASE(a)       Serial.println(a);
    #define PMM_DEBUG_PRINTF(a)             Serial.printf(a);
#else
    #define PMM_DEBUG_PRINT_BASE(x)          PMM_CANCEL_MACRO()
    #define PMM_DEBUG_PRINTLN_BASE(x)        PMM_CANCEL_MACRO()
#endif

    /*
    // Getting the FILENAME: https://stackoverflow.com/a/8488201/10247962
    #define PMM_DEBUG_PRINT(...)                     VA_SELECT(PMM_DEBUG_PRINT, __VA_ARGS__)
    #define PMM_DEBUG_PRINT_1(a)                     PMM_DEBUG_PRINT_BASE(a);
    #define PMM_DEBUG_PRINT_2(a, b)                { PMM_DEBUG_PRINT_1(a)          PMM_DEBUG_PRINT_BASE(b); }
    #define PMM_DEBUG_PRINT_3(a, b, c)             { PMM_DEBUG_PRINT_2(a, b)       PMM_DEBUG_PRINT_BASE(c); }
    #define PMM_DEBUG_PRINT_4(a, b, c, d)          { PMM_DEBUG_PRINT_3(a, b, c)    PMM_DEBUG_PRINT_BASE(d); }
    #define PMM_DEBUG_PRINT_5(a, b, c, d, e)       { PMM_DEBUG_PRINT_4(a, b, c, d) PMM_DEBUG_PRINT_BASE(e); }

    #define PMM_DEBUG_PRINTLN(...)                   VA_SELECT(PMM_DEBUG_PRINTLN, __VA_ARGS__)
    #define PMM_DEBUG_PRINTLN_0()                    PMM_DEBUG_PRINTLN_BASE("");
    #define PMM_DEBUG_PRINTLN_1(a)                   PMM_DEBUG_PRINTLN_BASE(a);
    #define PMM_DEBUG_PRINTLN_2(a, b)              { PMM_DEBUG_PRINTLN_1(a)          PMM_DEBUG_PRINTLN_BASE(b);  }
    #define PMM_DEBUG_PRINTLN_3(a, b, c)           { PMM_DEBUG_PRINTLN_2(a, b)       PMM_DEBUG_PRINTLN_BASE(c);  }
    #define PMM_DEBUG_PRINTLN_4(a, b, c, d)        { PMM_DEBUG_PRINTLN_3(a, b, c)    PMM_DEBUG_PRINTLN_BASE(d);  }
    #define PMM_DEBUG_PRINTLN_5(a, b, c, d, e)     { PMM_DEBUG_PRINTLN_4(a, b, c, d) PMM_DEBUG_PRINTLN_BASE(e);  }

     }
    #define PMM_DEBUG_ADV_PRINT_1(a)               { PMM_DEBUG_ADV_PRINT_0()           PMM_DEBUG_PRINT(a);   }
    #define PMM_DEBUG_ADV_PRINT_2(a, b)            { PMM_DEBUG_ADV_PRINT_1(a)          PMM_DEBUG_PRINT(b);   }
    #define PMM_DEBUG_ADV_PRINT_3(a, b, c)         { PMM_DEBUG_ADV_PRINT_2(a, b)       PMM_DEBUG_PRINT(c);   }
    #define PMM_DEBUG_ADV_PRINT_4(a, b, c, d)      { PMM_DEBUG_ADV_PRINT_3(a, b, c)    PMM_DEBUG_PRINT(d);   }
    #define PMM_DEBUG_ADV_PRINT_5(a, b, c, d, e)   { PMM_DEBUG_ADV_PRINT_4(a, b, c, d) PMM_DEBUG_PRINT(e);   }
*/
/*
    #define PMM_DEBUG_ADV_PRINTLN(...)               VA_SELECT(PMM_DEBUG_ADV_PRINTLN, __VA_ARGS__)
    #define PMM_DEBUG_ADV_PRINTLN_0()                PMM_DEBUG_ADV_PRINTLN("")
    #define PMM_DEBUG_ADV_PRINTLN_1(a)             { PMM_DEBUG_ADV_PRINT(a)          Serial.println(b); }
    #define PMM_DEBUG_ADV_PRINTLN_2(a, b)          { PMM_DEBUG_ADV_PRINT(a)          Serial.println(b); }
    #define PMM_DEBUG_ADV_PRINTLN_3(a, b, c)       { PMM_DEBUG_ADV_PRINT(a, b)       Serial.println(c); }
    #define PMM_DEBUG_ADV_PRINTLN_4(a, b, c, d)    { PMM_DEBUG_ADV_PRINT(a, b, c)    Serial.println(d); }
    #define PMM_DEBUG_ADV_PRINTLN_5(a, b, c, d, e) { PMM_DEBUG_ADV_PRINT(a, b, c, d) Serial.println(e); }
*/
    #define PMM_DEBUG_ADV_PRINT_0() { PMM_DEBUG_PRINT(FILENAME, " (", __func__, ", ln " STR2(__LINE__) ") : ") }
    #define PMM_DEBUG_ADV_PRINTF(x...) { PMM_DEBUG_ADV_PRINT_0() PMM_DEBUG_PRINTF(x) }


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
    #define PMM_IMU_DEBUG_PRINT_MORE(x) PMM_CANCEL_MACRO();
    #define PMM_IMU_DEBUG_PRINTLN_MORE(x) PMM_CANCEL_MACRO();
#endif



void printHexArray(uint8_t arrayToPrint[], unsigned arrayLength);


#endif