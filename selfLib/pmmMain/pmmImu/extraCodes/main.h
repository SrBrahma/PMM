#include <Arduino.h>
#include "pmmImu/extraCodes/calibration.h"

class PmmImuExtraCodes
{
    public:
        PmmImuExtraCodes()
        {
            int doQuit = 0;

            while (!doQuit)
            {
                Serial.println("=-= PMM IMU Extra Codes =-=");
                Serial.println("Press 1 to calibrate Accelerometer and Gyroscope (just let it stand still until Ok message is printed!");
                Serial.println("Press 2 to calibrate the Magnetometer");
                Serial.println("Anything else to quit\n");

                while (!Serial.available())
                    delay(50);

                switch(Serial.read())
                {
                    case '1':
                    {
                        Formatter* formatter = new Formatter;
                        formatter->setup();
                        delete formatter;
                        break;
                    }
                    // case '2':
                    // {
                    //     SafeLogTest* safeLogTest = new SafeLogTest;
                    //     safeLogTest->main();
                    //     delete safeLogTest;
                    //     break;
                    // }
                    default:
                        doQuit = 1;
                        break;
                }
            }
            Serial.println("=-= Finished PMM SD Extra Codes =-=\n");
        }
};