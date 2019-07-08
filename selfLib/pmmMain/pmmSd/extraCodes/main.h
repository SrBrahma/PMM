#include "pmmSd/extraCodes/formatter.h"
#include "pmmSd/extraCodes/safeLogTest.h"

class PmmSdExtraCodes
{
    public:
        PmmSdExtraCodes()
        {
            int doQuit = 0;

            while (!doQuit)
            {
                Serial.println("=-= PMM SD Extra Codes =-=");
                Serial.println("Press 1 to run the Formatting tool");
                Serial.println("Press 2 to run the SafeLog test");
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
                    case '2':
                    {
                        SafeLogTest* safeLogTest = new SafeLogTest;
                        safeLogTest->main();
                        delete safeLogTest;
                        break;
                    }
                    default:
                        doQuit = 1;
                        break;
                }
            }
            Serial.println("=-= Finished PMM SD Extra Codes =-=\n");
        }
};