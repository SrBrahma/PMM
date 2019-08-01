// This .h is for the specific Avionic uses.
// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#ifndef PMM_ROUTINES_ROCKET_AVIONIC_CONSTS_h
#define PMM_ROUTINES_ROCKET_AVIONIC_CONSTS_h

#define ROCKET_AVIONIC_PIN_DROGUE   36
#define ROCKET_AVIONIC_PIN_MAIN     35

// How much time each pin will be active after detecting it's time to deploy each recovery.
#define ROCKET_AVIONIC_DROGUE_ACTIVE_TIME_MS    3000
#define ROCKET_AVIONIC_MAIN_ACTIVE_TIME_MS      3000


#define MIN_PERCENTAGE_MEASURES_ANALYZER          95
#define LIFT_OFF_MIN_SPEED                         5
#define DROGUE_MIN_SPEED                          -5
// Set the INITIAL_ALTITUDE using the initial altitude from https://www.mapcoordinates.net/en
#define     INITIAL_ALTITUDE                     500

// Notice that the measured altitude uses the Mean Sea Level (MSL)
#define     MAIN_MIN_ALTITUDE                    500 + INITIAL_ALTITUDE
#define     MAIN_MIN_SPEED                        -5

// If the barometer detects this vertical velocity, means that the drogue didn't open.
#define     MAIN_NO_DROGUE_MIN_SPEED            -150

#endif