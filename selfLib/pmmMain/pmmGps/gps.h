/* pmmGps.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#ifndef PMM_GPS_h
#define PMM_GPS_h

#include <NMEAGPS.h>
#include "pmmGps/gpsType.h"
#include "pmmConsts.h"


// Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,

// Sleep mode https://forum.arduino.cc/index.php?topic=497410.15

class PmmGps
{

public:

    PmmGps();

    int init();
    int update();

    pmmGpsStructType  getGpsStruct   (); pmmGpsStructType* getGpsStructPtr();
    gps_fix           getFix         (); gps_fix*          getFixPtr      ();

private:
    void fixToOurType(gps_fix &mFix, pmmGpsStructType &mPmmGpsStruct);
    void debugPrintFix(Print &Serial, const NMEAGPS &mNMEAGPS, const gps_fix &mFix);

    bool    mGpsIsWorking;

    NMEAGPS mNMEAGPS; // This object parses received characters into the gps.fix() data structure

    gps_fix mFix;

    pmmGpsStructType mPmmGpsStruct;

    #if (defined GPS_FIX_SPEED && defined GPS_FIX_ALTITUDE) // https://stackoverflow.com/a/38474505
        unsigned long mLastReadMillis;
        unsigned long mTempLastReadMillis;
        float mLastAltitude;
    #endif

};

#endif
