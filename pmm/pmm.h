/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include <Arduino.h>

class Pmm
{
private:
    uint32_t mPackageTimeMs, mPackageLogId;

public:
    void init();
    void update();
};

#endif
