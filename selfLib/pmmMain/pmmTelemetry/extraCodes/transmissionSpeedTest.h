/* mPmmSdSafeLog->h
 *
 * Just a quick code to test the SafeLog system. This isn't intended at the moment to look good.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil
 * */


#ifndef TRANSMISSION_SPEED_TEST_h
#define TRANSMISSION_SPEED_TEST_h

#include <Arduino.h>
#include "pmmTelemetry/telemetry.h"

#include "pmmDebug.h"

#define TESTS_PER_LENGTH 1 // No need to send more than one!

#define MIN_LENGTH       0
#define MAX_LENGTH       PMM_TLM_MAX_PACKET_TOTAL_LENGTH

class TransmissionSpeedTest   // Being a class allows us to have functions inside, instead of global functions.
{

public:

    TransmissionSpeedTest() {}

    int main()
    {
        Serial.println("TransmissionSpeedTest, intializing...");

        if (init())
        {
            Serial.println("Fail at init(), quitting.\n");
            return 1;
        }

        PMM_DEBUG_PRINTF("Initialized successfully.\n\n");
        PMM_DEBUG_PRINTF("This test will send packets from lengths ranging from %i to %i (inclusive),\n", MIN_LENGTH, MAX_LENGTH);
        PMM_DEBUG_PRINTF("and will print the average time taken for each length in csv format, in milliseconds (but micros() is used).\n");
        PMM_DEBUG_PRINTF("The first value corresponds to the minimum length, and the last, to the maximum length.\n");
        PMM_DEBUG_PRINTF("The packet won't include any custom header, or any RadioHead lib header.\n");
        PMM_DEBUG_PRINTF("As some tests showed, there is no need to send more than one packet of the same length and make a average.\n");
        PMM_DEBUG_PRINTF("Results:\n\n");

        uint8_t data[MAX_LENGTH];
        uint32_t timeStart, timeSum;
        uint32_t totalTimeStart = millis();
        for (int length = MIN_LENGTH; length <= MAX_LENGTH; length++)
        {
            timeSum = 0;
            for (int counter = 0; counter < TESTS_PER_LENGTH; counter++)
            {
                while(!mPmmTelemetry.sendIfAvailableDebug(data, length)); // Keeps trying to send.
                timeStart = micros();
                while(mPmmTelemetry.isPacketBeingSent());
                timeSum += micros() - timeStart;
            }
            PMM_DEBUG_PRINTF("%.3f", timeSum / (TESTS_PER_LENGTH * 1000.0));
            if (length < MAX_LENGTH) PMM_DEBUG_PRINTF(",");
        } // End of while (!mDoQuit)
        PMM_DEBUG_PRINTF("\n\nEnded! Total time taken was %u seconds. Press any key to continue.\n", (millis() - totalTimeStart) / 1000);
        for (; !Serial.available(); delay(10));
        return 0;
    } // End of constructor

private:
    PmmTelemetry mPmmTelemetry;

    int init()
    {
        return mPmmTelemetry.init();
    }

};

#undef TESTS_PER_LENGTH
#undef MIN_LENGTH
#undef MAX_LENGTH

#endif