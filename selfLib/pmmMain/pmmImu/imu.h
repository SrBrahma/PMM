/* pmmImu.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#define PMM_CALIBRATE_BAROMETER_USING_INITIAL_ALTITUDE
#define PMM_ALTITUDE_AT_STARTING_PLACE

#ifndef PMM_IMU_h
#define PMM_IMU_h


#include <MPU6050.h>
#include <HMC5883L.h>
#include <Adafruit_BMP085_U.h>

#include <magDecByCoord.h>

#include <SimpleKalmanFilter.h>
#include <Plotter.h>

#include "pmmImu/imuType.h"
#include "pmmConsts.h"
#include "pmmDebug.h"



class PmmImu
{

public:
    PmmImu();

    int init();     // Must be executed, so the object is passed. Also, inits everything.
    int update();   // Gets all the sensors
    enum class ImuMode {Sleep, Active};
    int setImuMode(ImuMode mode);


    pmmImuStructType  getImuStruct();
    pmmImuStructType* getImuStructPtr();


    // MPU
    int    initMpu();
    int    updateMpu();
    void   getAccelerometer(float destinationArray[3]);
    void   getGyroscope(float destinationArray[3]);
    float  getMpuTemperature();

    float* getAccelerometerPtr();
    float* getGyroscopePtr();
    float* getMpuTemperaturePtr();
    // -=-=-=-=-=-

    // Barometer
    int    initBmp();
    int    updateBmp();

    int    setReferencePressure(unsigned samples = 10);

    float  getBarometer();
    float  getAltitudeBarometer();
    float  getBarometerTemperature();

    float* getBarometerPtr();
    float* getAltitudeBarometerPtr();
    float* getBarometerTemperaturePtr();
    // -=-=-=-=-=-


    // Magnetometer
    int    initMagnetometer();
    int    updateMagnetometer();

    int    setDeclination(float degrees);
    int    getDecByCoord (float* returnDeclination, float latitude, float longitude);
    int    setDeclination(float latitude, float longitude); // Uses coordinates to get declination, using another my another code.
    int    setDeclination(uint32_t latitude, uint32_t longitude); // Same as above, but using the uint32_t type for the coordinates.
    float  getDeclination();

    void   getMagnetometer(float destinationArray[3]);
    float* getMagnetometerPtr();
    // -=-=-=-=-=-



private:

    pmmImuStructType mPmmImuStruct;

    // MPU
    MPU6050  mMpu;
    int      mMpuIsWorking;
    // -=-=-=-=-=-

    // Barometer
    BMP085   mBarometer;
    double   mReferencePressure;
    int      mBarometerIsWorking;

    // Magnetometer
    HMC5883L mMagnetometer;
    int      mMagnetometerIsWorking;
    // -=-=-=-=-=-

};



#endif
