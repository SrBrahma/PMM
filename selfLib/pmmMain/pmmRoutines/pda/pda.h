#ifndef PDA_MAIN_h
#define PDA_MAIN_h

// Based on Layers.
// Layer 0  = Main application
// Layer 1+ = Popups

#include "pmm.h"

class PmmPda
{
public:
    int init(bool showSplash = true);
    int drawSplash(int display, int splashId);
    int blinkDisplay();
    int changeControlledDisplay();
private:
    uint8_t mControlledDisplay;
};

#endif