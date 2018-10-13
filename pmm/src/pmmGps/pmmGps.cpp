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
#include <pmmGps.h>
#include <pmmConsts.h>
#include <pmmErrorsCentral.h>

//-------------------------------------------------------------------------
//  The GPSport.h include file tries to choose a default serial port
//  for the GPS device.  If you know which serial port you want to use, edit the GPSport.h file.
#include <GPSport.h>

//------------------------------------------------------------
// For the NeoGPS example programs, "Streamers" is common set of printing and formatting routines for GPS data, in a
//   Comma-Separated Values text format (aka CSV).  The CSV data will be printed to the "debug output device".
// If you don't need these formatters, simply delete this section.
// #include <Streamers.h>

/* example from "trace_all(DEBUG_PORT, mGps, mFix);"
 Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,
3,2018-04-25 16:32:02.840,-228587517,-432291933,4830,5200,,,5,0,356,
3,2018-04-25 16:32:03.840,-228588150,-432292417,4930,3900,630,3,12,0,791,
3,2018-04-25 16:32:04.840,-228588617,-432292883,4600,2800,630,3,19,0,1226,
3,2018-04-25 16:32:05.840,-228588867,-432293117,4330,1700,630,3,26,0,1661,
*/

/* Also change GPSfix_cfg.h in neoGps Lib */

PmmGps::PmmGps(){}

int PmmGps::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mPmmErrorsCentral = pmmErrorsCentral;
    Serial1.begin(9600);
    #if PMM_GPS_GET_SPEEDS
        mTempLastReadMillis = 0;
        mLastReadMillis = 0;
    #endif
    return 0;
    PMM_DEBUG_PRINT_MORE("PmmGps: Initialized successfully!");
}

int PmmGps::update()
{
    if (mPmmErrorsCentral->getGpsIsWorking())
    {
        int hadUpdate = 0;
        while (mGps.available(gpsPort))
        {
            mFix = mGps.read();
            //doSomeWork();
            if (!hadUpdate)
                hadUpdate = 1;
        }

        if (hadUpdate)
        {
            mPmmGpsStruct.latitude = mFix.latitude();
            mPmmGpsStruct.longitude = mFix.longitude();

            #ifdef GPS_FIX_ALTITUDE
                mPmmGpsStruct.altitude = mFix.altitude();
            #endif

            #ifdef GPS_FIX_SATELLITES
                mPmmGpsStruct.satellites = mFix.satellites;
            #endif

            #ifdef GPS_FIX_SPEED
                mFix.calculateNorthAndEastVelocityFromSpeedAndHeading();
                mPmmGpsStruct.horizontalSpeed = mFix.speed_metersps();
                mPmmGpsStruct.northSpeed = mFix.velocity_northF();
                mPmmGpsStruct.eastSpeed = mFix.velocity_eastF();
                mPmmGpsStruct.headingDegree = mFix.heading();

                #ifdef GPS_FIX_ALTITUDE
                    mTempLastReadMillis = millis();
                    mLastAltitude = mPmmGpsStruct.altitude;
                    mPmmGpsStruct.upSpeed = ((mPmmGpsStruct.altitude - mLastAltitude) / ((mTempLastReadMillis - mLastReadMillis) / 1000.0)); // mFix.velocity_downF();
                    mLastReadMillis = mTempLastReadMillis;
                #endif
            #endif
        }

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
