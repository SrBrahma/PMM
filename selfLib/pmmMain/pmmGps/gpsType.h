#ifndef PMM_GPS_TYPE_h
#define PMM_GPS_TYPE_h

// This type will have almost all the things that the original NeoGps fix has. But, the original fix
// also has a lot of useless and confusing info for our needs, so we keep in this way, so it's clearer
// to whomever may change the PMM code.
// Also, makes the entire PMM code easier to mantain, in the case we change the GPS lib.
typedef struct
{
    int32_t   latitude;
    int32_t   longitude;

    float     altitude;
    uint8_t   satellites;

    float     headingDegree;

    float     horizontalSpeed;
    float     upSpeed;
    float     northSpeed;
    float     eastSpeed;
} pmmGpsStructType;

#endif