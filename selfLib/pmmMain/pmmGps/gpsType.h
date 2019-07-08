#ifndef PMM_GPS_TYPE_h
#define PMM_GPS_TYPE_h

// This type will have almost all the things that the original NeoGps fix has. But, the original fix
// also has a lot of useless and confusing info for our needs, so we keep in this way, so it's clearer
// to whomever may change the PMM code.
// Also, makes the entire PMM code easier to mantain, in the case we change the GPS lib.
typedef struct
{
    #ifdef GPS_FIX_LOCATION
        float   latitude;
        float   longitude;
    #endif

    #ifdef GPS_FIX_ALTITUDE
        float   altitude;
    #endif

    #ifdef GPS_FIX_SATELLITES
        uint8_t satellites;
    #endif

    #ifdef GPS_FIX_HEADING
        float   headingDegree;
    #endif

    #ifdef GPS_FIX_SPEED
        float   horizontalSpeed;
        float   upSpeed;
        #ifdef GPS_FIX_HEADING
            float northSpeed;
            float eastSpeed;
        #endif
    #endif

} pmmGpsStructType;

#endif