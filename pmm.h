#ifndef PMM_h
#define PMM_h

#include <Arduino.h>

class Pmm
{
private:
    uint32_t mPackageTimeMs, mPackageId;

public:
    void init();
    void update();
};

#endif
