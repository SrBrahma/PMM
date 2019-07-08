#ifndef PMM_IMU_TYPE_h
#define PMM_IMU_TYPE_h

typedef struct
{
    float accelerometerArray[3];    // Accelerations    of x,y,z, in m/s^2
    float gyroscopeArray[3];        // Angular velocity of x,y,z, in degrees/sec
    float temperatureMpu;

    float headingDegree;
    float headingRadian;

    float magnetometerArray[3];     // Magnetic fields  of x,y,z, in ..

    float pressure;
    float altitude;         // Relative altitude, to the starting altitude.
    float filteredAltitude;
    float temperatureBmp;

} pmmImuStructType;

#endif