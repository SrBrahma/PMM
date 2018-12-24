/* pmmImu.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */



#ifndef PMM_IMU_h
#define PMM_IMU_h


#include <MPU6050.h>
#include <HMC5883L.h>
#include <Adafruit_BMP085_U.h>

#include "pmmConsts.h"
#include "pmmDebug.h"



typedef struct
{
    float accelerometerArray[3];    // Accelerations    of x,y,z, in m/s^2
    float gyroscopeArray[3];        // Angular velocity of x,y,z, in degrees/sec
    float mpuTemperature;

    float headingDegree;
    float headingRadian;

    float magnetometerArray[3];     // Magnetic fields  of x,y,z, in ..

    float pressure;
    float altitudePressure;         // Relative altitude, to the starting altitude.
    float barometerTemperature;

} pmmImuStructType;

class PmmImu
{

public:
    PmmImu();
    /*
    int initAccelerometer();
    int initGyroscope(); */

    int init(); // Must be executed, so the object is passed. Also, inits everything.

    int update(); // Gets all the sensors

    int setReferencePressure(unsigned samples = 5);

    /* These returns safely a copy of the variables */
    void   getAccelerometer(float destinationArray[3]);
    void   getGyroscope(float destinationArray[3]);
    float  getMpuTemperature();
    void   getMagnetometer(float destinationArray[3]);
    float  getBarometer();
    float  getAltitudeBarometer();
    float  getBarometerTemperature();
    pmmImuStructType getImuStruct();

    /* These returns a pointer to the original variables - UNSAFE! Be careful! */
    float* getAccelerometerPtr();
    float* getGyroscopePtr();
    float* getMpuTemperaturePtr();
    float* getMagnetometerPtr();
    float* getBarometerPtr();
    float* getAltitudeBarometerPtr();
    float* getBarometerTemperaturePtr();
    pmmImuStructType* getImuStructPtr();

private:
    BMP085   mBarometer;
    MPU6050  mMpu;
    HMC5883L mMagnetometer;

    pmmImuStructType mPmmImuStruct;

    unsigned mMpuIsWorking;
    unsigned mBarometerIsWorking;
    unsigned mMagnetometerIsWorking;

    double   mReferencePressure;

    float    mMagnetometerDeclinationRad;



    int initMpu();
    int initMagnetometer();
    int initBmp();

    int updateMpu();
    int updateMagnetometer();
    int updateBmp();

};



#endif
