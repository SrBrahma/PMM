#include "pmmTelemetry/extraCodes/transmissionSpeedTest.h"
#include "pmmSd/extraCodes/safeLogTest.h"

class PmmTelemetryExtraCodes
{

public:
    PmmTelemetryExtraCodes()
    {
        int doQuit = 0;

        while (!doQuit)
        {
            Serial.println("=-= PMM Telemetry Extra Codes =-=");
            Serial.println("Type 1 to run the Transmission Speed Test");
            Serial.println("Anything else to quit\n");

            while (!Serial.available())
                delay(50);

            switch(Serial.read())
            {
                case '1':
                {
                    TransmissionSpeedTest* transmissionSpeedTest = new TransmissionSpeedTest;
                    transmissionSpeedTest->main();
                    delete transmissionSpeedTest;
                    break;
                }
                default:
                    doQuit = 1;
                    break;
            }
        }
        Serial.println("=-= Finished PMM Telemetry Extra Codes =-=\n");
    }
};