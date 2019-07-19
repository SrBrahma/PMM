#ifndef PMM_MODULES_SIMPLE_DATA_LOG_VARS_NAMES_h
#define PMM_MODULES_SIMPLE_DATA_LOG_VARS_NAMES_h

#include <Arduino.h> // Progmem

class VarsNames
{
    public:
    VarsNames();

    // To create a new default string, just use the sccc(constName) = "string".
    // If interested, google about 'static constexpr const char*'.
    #define sccc(x) static constexpr const char* x PROGMEM

    sccc (transmissionCounter   ) = "transmissionCounter";
    sccc (mainLoopCounter       ) = "mainLoopCounter";
    sccc (timeMillis            ) = "systemTime(ms)";

    // I couldn't find another way instead of decomposing the array.
    // Someone, have a better idea!
    sccc (accelerometerX) = "accelerometerX(g)";
    sccc (accelerometerY) = "accelerometerY(g)";
    sccc (accelerometerZ) = "accelerometerZ(g)";

    sccc (gyroscopeX) = "gyroscopeX(degree/s)";
    sccc (gyroscopeY) = "gyroscopeY(degree/s)";
    sccc (gyroscopeZ) = "gyroscopeZ(degree/s)";

    sccc (magnetometerX) = "magnetometerX(uT)";
    sccc (magnetometerY) = "magnetometerY(uT)";
    sccc (magnetometerZ) = "magnetometerZ(uT)";

    sccc (mpuTemperature        ) = "temperatureMpu(C)";

    sccc (barometerPressure     ) = "barometerPressure(hPa)";
    sccc (barometerAltitude     ) = "AltitudeBarometer(m)";
    sccc (bmpTemperature        ) = "temperatureBmp(C)";

    sccc (gpsLatitude           ) = "gpsLatitude";
    sccc (gpsLongitude          ) = "gpsLongitude";
    sccc (gpsAltitude           ) = "gpsAltitude(m)";
    sccc (gpsSatellites         ) = "gpsSatellites";
    sccc (gpsHeadingDegree      ) = "gpsHeadingDegree";

    sccc (gpsSpeedUp            ) = "gpsSpeedUp(m/s)";
    sccc (gpsSpeedHorizontal    ) = "gpsHorizontalSpeed(m/s)";
    sccc (gpsSpeedNorth         ) = "gpsNorthSpeed(m/s)";
    sccc (gpsSpeedEast          ) = "gpsEastSpeed(m/s)";

    #undef sccc
};

#endif