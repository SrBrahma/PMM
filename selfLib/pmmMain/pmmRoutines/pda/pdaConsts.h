// This .h is for the specific PDA uses.
// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#ifndef PMM_ROUTINES_PDA_CONSTS_h
#define PMM_ROUTINES_PDA_CONSTS_h


// The default lib doesn't allow different channels. No will to change the lib now.
// #define PDA_CHAR_DISPLAY_I2C_CHANNEL

#define PDA_DISPLAY_MIN_TIME_MS_BETWEEN_UPDATES     250
#define PDA_DISPLAY_ADDRESS                        0x27
#define PDA_DISPLAY_COLUMNS                          20
#define PDA_DISPLAY_ROWS                              4

#endif