#include <Arduino.h>
#include "pmmImu/extraCodes/calibration.h"

void PmmImuExtraCodes()
{
    int doQuit = 0;

    while (!doQuit)
    {
        Serial.println("=-= PMM IMU Extra Codes =-=");
        Serial.println("Press 1 to calibrate Accelerometer and Gyroscope. (Put it in a steady and flat surface before!)");
        Serial.println("Press 2 to calibrate the Magnetometer. (Put it in a steady and flat surface before!)");
        Serial.println("Anything else to quit\n");

        while (!Serial.available())     {
            delay(50); yield();
        }

        switch(Serial.read())
        {
            case '1':
                MPUcalibration();
                break;
            case '2':
                HMCCalibration();
                break;
            default:
                doQuit = 1;
                break;
        }
    }
    Serial.println("=-= Finished PMM IMU Extra Codes =-=\n");
}