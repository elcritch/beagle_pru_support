
#include "../spi-devs.h"

#include <pru_support_lib.h>

#include <stdlib.h>

static inline void delayCycles(int cycles)
{
  for (int i=0; i++; i<cycles)
    __asm__("nop\n\t");
  // __nop();
}

void spi_delay(uint32_t time) { while (--time) delayCycles(10); }



int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {

    return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}

