/* pmmGps.cpp
 * Some commentaries were taken from the neoGps lib.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

//======================================================================
//  Program: NMEA.ino
//
//  Description:  This program uses the fix-oriented methods available() and
//    read() to handle complete fix structures.
//
//    When the last character of the LAST_SENTENCE_IN_INTERVAL (see NMEAGPS_cfg.h)
//    is decoded, a completed fix structure becomes available and is returned
//    from read().  The new fix is saved the 'fix' structure, and can be used
//    anywhere, at any time.
//
//    If no messages are enabled in NMEAGPS_cfg.h, or
//    no 'gps_fix' members are enabled in GPSfix_cfg.h, no information will be
//    parsed, copied or printed.
//
//  Prerequisites:
//     1) Your GPS device has been correctly powered.
//          Be careful when connecting 3.3V devices.
//     2) Your GPS device is correctly connected to an Arduino serial port.
//          See GPSport.h for the default connections.
//     3) You know the default baud rate of your GPS device.
//          If 9600 does not work, use NMEAdiagnostic.ino to
//          scan for the correct baud rate.
//     4) LAST_SENTENCE_IN_INTERVAL is defined to be the sentence that is
//          sent *last* in each update interval (usually once per second).
//          The default is NMEAGPS::NMEA_RMC (see NMEAGPS_cfg.h).  Other
//          programs may need to use the sentence identified by NMEAorder.ino.
//     5) NMEAGPS_RECOGNIZE_ALL is defined in NMEAGPS_cfg.h
//
//======================================================================

#include <Arduino.h>
#include <NMEAGPS.h>
#include "pmmConsts.h"
#include "pmmDebug.h"

#include "pmmGps/pmmGps.h"

//------------------------------------------------------------
#if PMM_DEBUG && PMM_DEBUG_MORE && PMM_GPS_DEBUG_MORE
    #include <Streamers.h>
    // example from "trace_all(DEBUG_PORT, mGps, mFix);"
    // Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,
    // 3,2018-04-25 16:32:02.840,-228587517,-432291933,4830,5200,,,5,0,356,
    // 3,2018-04-25 16:32:05.840,-228588867,-432293117,4330,1700,630,3,26,0,1661,
#endif

PmmGps::PmmGps(){}

int PmmGps::init()
{
    
    Serial1.begin(9600);
    #if PMM_GPS_GET_SPEEDS
        mTempLastReadMillis = 0;
        mLastReadMillis = 0;
    #endif

    if (Serial1)
    {
        gpsDebugMorePrintf("Initialized successfully!\n");
        mGpsIsWorking = 1;
    }

    else
    {
        advPrintf("Initialization failed!\n");
        mGpsIsWorking = 0;
    }

    return 0;
}

int PmmGps::update()
{
    if (mGpsIsWorking)
    {
        int hadUpdate = 0;
        while (mGps.available(PMM_GPS_PORT))
        {
            mFix = mGps.read();
            //doSomeWork();
            if (!hadUpdate)
                hadUpdate = 1;
        }

        if (hadUpdate)
        {
            #ifdef GPS_FIX_LOCATION
                mPmmGpsStruct.latitude = mFix.latitude();
                mPmmGpsStruct.longitude = mFix.longitude();
            #endif

            #ifdef GPS_FIX_ALTITUDE
                mPmmGpsStruct.altitude = mFix.altitude();
            #endif

            #ifdef GPS_FIX_SATELLITES
                mPmmGpsStruct.satellites = mFix.satellites;
            #endif

            #ifdef GPS_FIX_HEADING
                mPmmGpsStruct.headingDegree   = mFix.heading();
            #endif

            #ifdef GPS_FIX_SPEED
                mPmmGpsStruct.upSpeed           = mFix.velocity_down    / (-100.0);  // As NeoGps outputs this in cm/s, we divide by 100 to get in m/s.
                mFix.calculateNorthAndEastVelocityFromSpeedAndHeading();
                mPmmGpsStruct.horizontalSpeed   = mFix.speed_metersph() / 3600.0  ;  // In m/s
                #ifdef GPS_FIX_HEADING
                    mPmmGpsStruct.northSpeed    = mFix.velocity_north   / 100.0   ;  // As NeoGps outputs this in cm/s, we divide by 100 to get in m/s.
                    mPmmGpsStruct.eastSpeed     = mFix.velocity_east    / 100.0   ;  // As NeoGps outputs this in cm/s, we divide by 100 to get in m/s.
                #endif
            #endif

        }
        #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_GPS_DEBUG_MORE
            gpsDebugMorePrintf("trace_all():")
            trace_all(Serial, mGps, mFix);
            delay(10000);
        #endif
        return hadUpdate;
    }
    else
        return 0;
}

pmmGpsStructType* PmmGps::getGpsStructPtr()
{
    return &mPmmGpsStruct;
}

pmmGpsStructType PmmGps::getGpsStruct()
{
    return mPmmGpsStruct;
}
//----------------------------------------------------------------
//  This function gets called about once per second, during the GPS quiet time.  It's the best place to do anything that might take
//  a while: print a bunch of things, write to SD, send an SMS, etc.
//  By doing the "hard" work during the quiet time, the CPU can get back to reading the GPS chars as they come in, so that no chars are lost.

//void PmmGps::doSomeWork()
//{
  // Print all the things!
  //trace_all(DEBUG_PORT, mGps, mFix);
//} // doSomeWork



//--------------------------
/*
void setup()
{

  DEBUG_PORT.print( F("NMEA.INO: started\n") );
  DEBUG_PORT.print( F("  fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("  gps object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );

  #ifndef NMEAGPS_RECOGNIZE_ALL
    #error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
  #endif

  #ifdef NMEAGPS_INTERRUPT_PROCESSING
    #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
  #endif

  #if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )

    DEBUG_PORT.println( F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed.") );

  #else
    if (gps.merging == NMEAGPS::NO_MERGING) {
      DEBUG_PORT.print  ( F("\nWARNING: displaying data from ") );
      DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      DEBUG_PORT.print  ( F(" sentences ONLY, and only if ") );
      DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      DEBUG_PORT.println( F(" is enabled.\n"
                            "  Other sentences may be parsed, but their data will not be displayed.") );
    }
  #endif

  DEBUG_PORT.print  ( F("\nGPS quiet time is assumed to begin after a ") );
  DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
  DEBUG_PORT.println( F(" sentence is received.\n"
                        "  You should confirm this with NMEAorder.ino\n") );

  trace_header( DEBUG_PORT );
  DEBUG_PORT.flush();

  gpsPort.begin( 9600 );
}
*/
