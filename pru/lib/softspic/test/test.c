
#include "acutest.h"

// #define NOOP delay_test_cycles()
#define PRU_SUPPORT_OVERRIDE_GPIO_FUNCS

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
  SpiPins_t iopins;
};
typedef struct SimpleCycleTiming SCT_t;

SCT_t* simplecycletiming_init() {
  SCT_t *t = malloc(sizeof(SCT_t));
  t->cycle = 0;

  return t;
}

void sct_set_pin(SCT_t *this, Pin pin, uint32_t state) {
  if (pin == this->iopins.mosi) {
    this->current.mosi = state;
    this->current.miso = !state;
  }
  else if (pin == this->iopins.miso)
    this->current.miso = state;
  else if (pin == this->iopins.sck)
    this->current.sck = state;
  else {
    this->current.other_pin = pin;
    this->current.other_state = state;
  }
}

bool sct_get_pin(SCT_t *this, Pin pin) {
  if (pin == this->iopins.mosi)
    return this->current.mosi;
  else if (pin == this->iopins.miso)
  {
      uint8_t ret = this->current.miso;
      this->current.miso = 2+ret;
      return ret;
  }
  else if (pin == this->iopins.sck)
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


// Hardware Emulation
void delay_test_cycles();
void digitalWrite(uint32_t gpio_bitmask, bool state);
bool digitalRead(uint32_t gpio_bitmask);
void digitalToggle(uint32_t gpio_bitmask);

SpiPins_t iopins = {
  .miso = 10,
  .mosi = 11,
  .sck = 14,
  .sck_dir = 0,
};
SpiPins_t iopins_inv = {
  .miso = 10,
  .mosi = 11,
  .sck = 14,
  .sck_dir = 1,
};


SCT_t _cycle_data = {
  .cycle = 0,
  .iopins = {
    .miso = 10,
    .mosi = 11,
    .sck = 14,
    .sck_dir = 0,
  },
};

SCT_t* cycle_data = &_cycle_data;

void delay_test_cycles() {
  sct_incr(cycle_data);
}

void digitalWrite(uint32_t gpio_bitmask, bool state) {
  sct_set_pin(cycle_data, gpio_bitmask, state);
  // cycle_data.incr();
}
void digitalToggle(uint32_t gpio_bitmask) {
  /* cycle_data->set_pin(gpio_bitmask, !cycle_data->get_pin(gpio_bitmask)); */
  sct_set_pin(cycle_data, gpio_bitmask, !sct_get_pin(cycle_data, gpio_bitmask));
  // cycle_data.incr();
}
bool digitalRead(uint32_t gpio_bitmask) {
  bool res = sct_get_pin(cycle_data, gpio_bitmask);
  /* bool res = cycle_data->get_pin(gpio_bitmask); */
  // cycle_data.incr();
  return res;
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

