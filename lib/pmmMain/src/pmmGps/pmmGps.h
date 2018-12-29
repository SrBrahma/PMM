/* pmmGps.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_GPS_h
#define PMM_GPS_h

#include <NMEAGPS.h>

#include "pmmConsts.h"


// Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,

typedef struct
{
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

    #ifdef GPS_FIX_HEADING
        float headingDegree;
    #endif

    #ifdef GPS_FIX_SPEED
        float horizontalSpeed;
        float upSpeed;
        #ifdef GPS_FIX_HEADING
            float northSpeed;
            float eastSpeed;
        #endif
    #endif

} pmmGpsStructType;



class PmmGps
{

public:
    PmmGps();

    int init();
    int update();

    // Sleep mode https://forum.arduino.cc/index.php?topic=497410.15
    
    pmmGpsStructType* getGpsStructPtr();
    pmmGpsStructType getGpsStruct();
    //void doSomeWork();

private:
    unsigned mGpsIsWorking;
    
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
    pmmGpsStructType mPmmGpsStruct;
    

};

#endif
