#ifndef PMM_GPS_TYPE_h
#define PMM_GPS_TYPE_h

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

#endif