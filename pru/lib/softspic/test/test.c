
#define MUNIT_ENABLE_ASSERT_ALIASES


#include "munit.h"
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

static MunitResult
test_compare(const MunitParameter params[], void* data) {
  /* We'll use these later */
  const unsigned char val_uchar = 'b';
  const short val_short = 1729;
  double pi = 3.141592654;
  char* stewardesses = "stewardesses";
  char* most_fun_word_to_type;

  /* These are just to silence compiler warnings about the parameters
   * being unused. */
  (void) params;
  (void) data;

  /* Let's start with the basics. */
  munit_assert(0 != 1);

  /* There is also the more verbose, though slightly more descriptive
     munit_assert_true/false: */
  /* munit_assert_false(0); */

  /* munit_assert_char('a', ==, 'a'); */
  /* munit_assert_uchar(val_uchar, ==, 'b'); */
  /* munit_assert_short(42, <, val_short); */
  /* munit_assert_size(strlen("uncopyrightables"), >, strlen("dermatoglyphics")); */
  /* munit_assert_double(pi, ==, 3.141592654); */
  /* munit_assert_double_equal(3.141592654, 3.141592653589793, 9); */
  /* munit_assert_string_equal(stewardesses, "stewardesses"); */
  /* munit_assert_memory_equal(7, stewardesses, "steward"); */
  /* munit_assert_memory_not_equal(8, stewardesses, "steward"); */
  /* most_fun_word_to_type = stewardesses; */
  /* munit_assert_ptr_equal(most_fun_word_to_type, stewardesses); */
  /* munit_assert_null(NULL); */
  /* munit_assert_not_null(most_fun_word_to_type); */
  /* munit_assert_ptr_equal(data, (void*)(uintptr_t)0xdeadbeef); */

  return MUNIT_OK;
}


static MunitTest test_suite_tests[] = {
  {(char*) "/test/spi", test_compare, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite test_suite = {
  (char*) "",
  test_suite_tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};



int main(int argc, char** argv) {
	  /* assert_int(argc, ==, 1); */

    return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}

