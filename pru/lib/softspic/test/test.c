
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


#include "unity.h"
#include "DumbExample.h"

void test_AverageThreeBytes_should_AverageMidRangeValues(void)
{
  TEST_ASSERT_EQUAL_HEX8(40, AverageThreeBytes(30, 40, 50));
  TEST_ASSERT_EQUAL_HEX8(40, AverageThreeBytes(10, 70, 40));
  TEST_ASSERT_EQUAL_HEX8(33, AverageThreeBytes(33, 33, 33));
}

void test_AverageThreeBytes_should_AverageHighValues(void)
{
  TEST_ASSERT_EQUAL_HEX8(80, AverageThreeBytes(70, 80, 90));
  TEST_ASSERT_EQUAL_HEX8(127, AverageThreeBytes(127, 127, 127));
  TEST_ASSERT_EQUAL_HEX8(84, AverageThreeBytes(0, 126, 126));
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_AverageThreeBytes_should_AverageMidRangeValues);
  RUN_TEST(test_AverageThreeBytes_should_AverageHighValues);
  return UNITY_END();
}
