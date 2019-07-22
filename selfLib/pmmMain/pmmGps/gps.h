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

    enum class UpdateRtn {NotWorking, OkNoData, GotFix};
    UpdateRtn update(uint32_t timeMs);

    pmmGpsStructType  getGpsStruct   (); pmmGpsStructType*  getGpsStructPtr();
    gps_fix           getFix         (); gps_fix*           getFixPtr      ();
    uint32_t    getLastLocationTimeMs(); uint32_t* getLastLocationTimeMsPtr();

    // For now only accepts lat and lon in int32_t format. It's the best option actually.
    // Uses 'this' current position.
    double distanceToInMeters(int32_t targetLatitude, int32_t targetLongitude);
    float  bearingToInDegrees(int32_t targetLatitude, int32_t targetLongitude);

private:
    void fixToOurType(gps_fix &mFix, pmmGpsStructType &mPmmGpsStruct);
    void debugPrintFix(Print &Serial, const NMEAGPS &mNMEAGPS, const gps_fix &mFix);

    bool    mGpsIsWorking;

    NMEAGPS mNMEAGPS; // This object parses received characters into the gps.fix() data structure

    gps_fix mFix;

    uint32_t mLastLocationTimeMs;
    pmmGpsStructType mPmmGpsStruct;

};

#endif
