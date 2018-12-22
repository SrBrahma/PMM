#include "pmm.h"
#include "pmmExtraCodes.h"

Pmm pmm;

void setup()
{
    runExtraCodes();
    pmm.init();
    //expansion.init();
}

void loop()
{
    pmm.update();
    //expansion.update();

}
