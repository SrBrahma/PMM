#ifndef PMM_GPS_h
#define PMM_GPS_h

#include <pmmConsts.h>
#include "neoGps/NMEAGPS.h"
#include <pmmErrorsAndSignals.h>

// Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,

class PmmGps
{
private:
    NMEAGPS mGps; // This object parses received characters into the gps.fix() data structure

    #if (defined GPS_FIX_SPEED && defined GPS_FIX_ALTITUDE) // https://stackoverflow.com/a/38474505
        unsigned long mLastReadMillis;
        unsigned long mTempLastReadMillis;
        float mLastAltitude;
    #endif
    //------------------------------------------------------------
    //  Define a set of GPS fix information.  It will hold on to the various pieces as they are received from
    //  an RMC sentence.  It can be used anywhere in your sketch.
    gps_fix mFix;
    // gpsStructType mgpsStructType;
    PmmErrorsAndSignals *mPmmErrorsAndSignals;

public:
    PmmGps();
    int init(PmmErrorsAndSignals *pmmErrorsAndSignals);
    int update();
    //void doSomeWork();

    #ifdef GPS_FIX_LOCATION
        float latitude;
        float longitude;
    #endif

    #ifdef GPS_FIX_ALTITUDE
        float altitude;
    #endif

    #ifdef GPS_FIX_SATELLITES
        uint8_t satellites;
    #endif

    #ifdef GPS_FIX_SPEED
        float horizontalSpeed;
        float northSpeed;
        float eastSpeed;
        float headingDegree;

        #ifdef GPS_FIX_ALTITUDE
            float upSpeed;
        #endif
    #endif
};

#endif
