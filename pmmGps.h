#ifndef PMM_GPS_h
#define PMM_GPS_h

#include <pmmConsts.h>
#include "neoGps/NMEAGPS.h"
// Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,

typedef struct // Speed are in meters/s
{
    float latitude;
    float longitude;
    float altitude;
    uint8_t satellites;
    #if PMM_GPS_GET_SPEEDS
        unsigned long lastReadMillis;
        float horizontalSpeed;
        float speedNorth;
        float speedEast;
        float speedUp;
        float headingDegree;
    #endif
} Gps_structType; //IMU Structure

class GpsManager
{
private:
    //------------------------------------------------------------
    // This object parses received characters
    //   into the gps.fix() data structure
    NMEAGPS mGps;
    #if PMM_GPS_GET_SPEEDS
        unsigned long mTempLastReadMillis;
        float mLastAltitude;
    #endif
    //------------------------------------------------------------
    //  Define a set of GPS fix information.  It will hold on to the various pieces as they are received from
    //  an RMC sentence.  It can be used anywhere in your sketch.
    gps_fix mFix;
    // Gps_structType mGps_struct;

public:
    GpsManager();
    int init();
    int update(Gps_structType *gps_struct);
    //void doSomeWork();
};

#endif
