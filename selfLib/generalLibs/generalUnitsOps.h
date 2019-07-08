#ifndef GENERAL_UNITS_OPS_h
#define GENERAL_UNITS_OPS_h

#include <stdint.h> // For uint32_t

// Converts the latitude or longitude in uint32_t type to float type.
inline float coord32ToFloat(uint32_t latOrLon)
{
    return latOrLon * 1.0e-7; // That's how it happens on Location.h, of NeoGps lib.
}

inline uint32_t secondsToMicros(float seconds) { return seconds * 1000000; }
inline uint32_t secondsToMillis(float seconds) { return seconds *    1000; }
inline uint32_t millisToMicros (float millis ) { return millis  *    1000; }

// Also check time difference if the time had overflowed.
uint32_t timeDifference(uint32_t newTime, uint32_t oldTime)
{
    if (newTime < oldTime) // If the time has 
        return (0xFFFFFFFF - oldTime + newTime);

    return (newTime - oldTime);
}
#endif