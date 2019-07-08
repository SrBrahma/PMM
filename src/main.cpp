#include "pmm.h"

Pmm pmm;

void setup()
{
    pmm.init();
}

void loop()
{
    pmm.update();
}
