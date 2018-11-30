#include "pmmSd/pmmSd.h"
#include "pmmSd/pmmSdSafeLog.h"

#include "pmmSd/extraCodes/formatter.h"

class PmmSdExtraCodes
{
    public:
        PmmSdExtraCodes()
        {
            Serial.println("=-= PmmSdExtraCodes =-=");
            Serial.println("Type 1 to run the Formatting tool");
            Serial.println("Type 2 to run the SafeLog test");
            Serial.println("Anything else to quit this test");

            while (!Serial.available())
                delay(50);

            switch(Serial.read())
            {
                case '1':
                    Formatter formatter;
                    formatter.setup();
            }
            delete this;
        }
};