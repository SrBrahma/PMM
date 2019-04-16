/* pmmGps.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_GPS_h
#define PMM_GPS_h

#include <NMEAGPS.h>
#include "pmmGpsType.h"
#include "pmmConsts.h"


// Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,


class PmmGps
{

public:
    PmmGps();

    int init();
    int update();

    // Sleep mode https://forum.arduino.cc/index.php?topic=497410.15
    
    pmmGpsStructType* getGpsStructPtr();
    pmmGpsStructType  getGpsStruct();
    gps_fix*          getFixPtr();

    //void doSomeWork();

private:
    unsigned mGpsIsWorking;
    
    NMEAGPS mGps; // This object parses received characters into the gps.fix() data structure

    #if (defined GPS_FIX_SPEED && defined GPS_FIX_ALTITUDE) // https://stackoverflow.com/a/38474505
        unsigned long mLastReadMillis;
        unsigned long mTempLastReadMillis;
        float mLastAltitude;
    #endif

    gps_fix mFix;
    pmmGpsStructType mPmmGpsStruct;

};

#endif
