/* pmmImu.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */



#ifndef PMM_IMU_h
#define PMM_IMU_h

#define DELAY_MS_BAROMETER 100 //random value

#define PMM_IMU_DEBUG       1
#define PMM_IMU_DEBUG_MORE  1 // For this to work, 

#if PMM_IMU_DEBUG_MORE
    #define PMM_IMU_DEBUG_PRINT_MORE(x) PMM_DEBUG_PRINTLN_MORE(x)
#else
    #define PMM_IMU_DEBUG_PRINT_MORE(x) PMM_CANCEL_MACRO(x)
#endif

#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>

#include "pmmConsts.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"

typedef struct
{
    float accelerometerArray[3];    // Accelerations    of x,y,z, in m/s^2
    float magnetometerArray[3];     // Magnetic fields  of x,y,z, in ..
    float gyroscopeArray[3];        // Angular velocity of x,y,z, in degrees/sec
    float pressure;
    float altitudePressure;         // Relative altitude, to the starting altitude.
    float temperature;
    float headingDegree;
    float headingRadian;
} pmmImuStructType;

class PmmImu
{

public:
    PmmImu();
    /*
    int initAccelerometer();
    int initGyroscope(); */

    int init(PmmErrorsCentral *pmmErrorsCentral); // Must be executed, so the object is passed. Also, inits everything.

    int update(); // Gets all the sensors

    /* These returns safely a copy of the variables */
    void  getAccelerometer(float destinationArray[3]);
    void  getGyroscope(float destinationArray[3]);
    void  getMagnetometer(float destinationArray[3]);
    float getBarometer();
    float getAltitudeBarometer();
    float getTemperature();
    pmmImuStructType getImuStruct();

    /* These returns a pointer to the original variables - UNSAFE! Be careful! */
    float* getAccelerometerPtr();
    float* getGyroscopePtr();
    float* getMagnetometerPtr();
    float* getBarometerPtr();
    float* getAltitudeBarometerPtr();
    float* getTemperaturePtr();
    pmmImuStructType* getImuStructPtr();

private:
    BMP085 mBarometer;
    MPU6050 mMpu;
    HMC5883L mMagnetometer;

    PmmErrorsCentral *mPmmErrorsCentral;

    pmmImuStructType mPmmImuStruct;

    float mMagnetometerDeclinationRad;

    unsigned long mNextMillisBarometer;

    double mReferencePressure;

    int initMpu();
    int initMagnetometer();
    int initBmp();

    int updateMpu();
    int updateMagnetometer();
    int updateBmp();

};



#endif
