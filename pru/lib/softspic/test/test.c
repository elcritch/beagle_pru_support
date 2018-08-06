
#include "acutest.h"

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


#include <stdint.h>
#include <string.h>
#include <stdio.h>

volatile uint32_t __R30;
volatile uint32_t __R31;

// #define DEBUG_STMT
// void debug(std::string msg);

#define MAX_CYCLES 4096

typedef uint32_t Pin;

struct PinState {
  int miso;
  int mosi;
  int sck;

  uint32_t other_pin;
  int other_state;

  uint32_t cycle;
};

typedef struct PinState PS_t;

PS_t PinState() {
  PS_t ps = {
    .miso = false,
    .mosi = false,
    .sck = false,
    .other_pin = 0,
    .other_state = true,
  };

  return ps;
}

void print(PS_t ps) {
  printf("PinState: ");
  printf("miso = %d,", ps.miso);
  printf("mosi = %d,", ps.mosi);
  printf("sck = %d,", ps.sck);
  printf("x-pin = %d,", ps.other_pin);
  printf("x-state = %d,", ps.other_state);
  printf("cycle = %d,", ps.cycle);
  printf("\n");
}


#define MAX_CYCLES 4096

struct SimpleCycleTiming {
  
  PS_t states[MAX_CYCLES];
  uint64_t cycle;
  PS_t current;
};
typedef struct SimpleCycleTiming SCT_t;

SCT_t* simplecycletiming_init() {
  SCT_t *t = malloc(sizeof(SCT_t));
  t->cycle = 0;

  return t;
}

void sct_set_pin(SCT_t *this, Pin pin, uint32_t state, SpiPins_t iopins) {
  if (pin == iopins.mosi) {
    this->current.mosi = state;
    this->current.miso = !state;
  }
  else if (pin == iopins.miso)
    this->current.miso = state;
  else if (pin == iopins.sck)
    this->current.sck = state;
  else {
    this->current.other_pin = pin;
    this->current.other_state = state;
  }
}

bool sct_get_pin(SCT_t *this, Pin pin, SpiPins_t iopins) {
  if (pin == iopins.mosi)
    return this->current.mosi;
  else if (pin == iopins.miso)
  {
      uint8_t ret = this->current.miso;
      this->current.miso = 2+ret;
      return ret;
  }
  else if (pin == iopins.sck)
    return this->current.sck;
  else if (pin == this->current.other_pin) {
    uint8_t ret =  this->current.other_state;
    this->current.other_state = 2;
    return ret;
  }
  else
  {
    printf("pin error: %d other_pin: %d sck: %d", pin, this->current.other_pin, this->current.sck);
  }
}

void sct_incr(SCT_t *this) {
    this->states[this->cycle++] = this->current;
    this->current.cycle = this->cycle;
}


void test_example(void)
{
  int a = 1;
  int b = 2;
  TEST_CHECK_(a + b == 3, "Expected %d, got %d", 3, a + b);

  /* TEST_CHECK_(a + 2*b == 3, "Expected %d, got %d", 3, a + 2*b); */
}

TEST_LIST = {
  { "example", test_example },
  { 0 }
};

