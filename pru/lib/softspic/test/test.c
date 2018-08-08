

#include "acutest.h"

// #define NOOP delay_test_cycles()
/* #define PRU_SUPPORT_OVERRIDE_GPIO_FUNCS */

#include "../spi-devs.h"

#include <pru_support_lib.h>

#include <stdlib.h>
#include <string.h>

static inline void delayCycles(int cycles)
{
  for (int i=0; i++; i<cycles)
    __asm__("nop\n\t");
  // __nop();
}



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
  SCT_t *t = calloc(sizeof(SCT_t),1);
  t->cycle = 0;

  return t;
}

void sct_set_pin(SCT_t *this, Pin pin, bool state) {
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


SCT_t* cycle_data = NULL;

void delay_test_cycles() {
  sct_incr(cycle_data);
}

void spi_delay(uint32_t time) {
  while (--time) delayCycles(10);
  sct_incr(cycle_data);
}

void digitalWrite(uint32_t gpio_bitmask, bool state) {
  sct_set_pin(cycle_data, gpio_bitmask, state);
  // cycle_data.incr();
}
/* void digitalToggle(uint32_t gpio_bitmask) { */
/*   /\* cycle_data->set_pin(gpio_bitmask, !cycle_data->get_pin(gpio_bitmask)); *\/ */
/*   sct_set_pin(cycle_data, gpio_bitmask, !sct_get_pin(cycle_data, gpio_bitmask)); */
/*   // cycle_data.incr(); */
/* } */
bool digitalRead(uint32_t gpio_bitmask) {
  bool res = sct_get_pin(cycle_data, gpio_bitmask);
  /* bool res = cycle_data->get_pin(gpio_bitmask); */
  // cycle_data.incr();
  return res;
}


void printCycleDataFull() {
  printf("Done...\n");
  for (int idx = 0; idx < cycle_data->cycle; idx++)
    print(cycle_data->states[idx]);
  printf("\n\n");
}

/* std::cout<<std::bitset<8>(out)<<std::endl; */
void printBits(uint32_t data, uint8_t count) {
  printf("0b");
  for (uint8_t i = 0; i < count; i++) {
    printf("%c", (data >> i & 0x1) ? '1' : '0');
  }
  printf(" ");
}

struct cycle_out {
  char input[128];
  char sck[128];
  char mosi[128];
  char miso[128];
  char other[128];
};
typedef struct cycle_out cycle_out_t;

cycle_out_t printCycleData(uint32_t input, uint8_t count, uint8_t out) {
  struct cycle_out co;

  for (int i=0; i < count; i++) {
    sprintf(&co.input[i], "%1d", input >> i & 0x1);
  }
  for (int i=0; i < cycle_data->cycle; i++) {
    sprintf(&co.sck[i], "%1d", cycle_data->states[i].sck);
  }
  for (int i=0; i < cycle_data->cycle; i++) {
    sprintf(&co.mosi[i], "%1d", cycle_data->states[i].mosi);
  }
  for (int i=0; i < cycle_data->cycle; i++) {
    sprintf(&co.miso[i], "%1d", cycle_data->states[i].miso);
  }
  for (int i=0; i < cycle_data->cycle; i++) {
    sprintf(&co.other[i], "%1d", cycle_data->states[i].other_state);
  }

  printf("\ninput:  %s", (char*)&co.input);
  printf("\nsck:    %s", (char*)&co.sck);
  printf("\nmosi:   %s", (char*)&co.mosi);
  printf("\nmiso:   %s", (char*)&co.miso);
  printf("\nother:  %s", (char*)&co.other);
  printf("\nno_cycles: %lu, read out: %d \n", cycle_data->cycle, out);
  printf("\n");
  /* std::cout<<std::bitset<8>(out)<<std::endl; */
  printBits(out, 8);

  return co;
};

SpiTimings_t timings = {
  .pre = 7,
  .c0 = 5,
  .c1 = 5,
  .post = 7,
};

#define TEST_STR(a, b) \
  TEST_CHECK_(strcmp(a,b) == 0, "expected: %s found: %s", b, a);

/* extern uint32_t spi_xfer_cpha0(uint32_t data, uint8_t bit_count, SpiPins_t pins, SpiTimings_t timings); */
/* extern  uint32_t spi_xfer_cpha1(uint32_t data, uint8_t bit_count, SpiPins_t pins, SpiTimings_t timings); */
void test_example(void)
{
  int a = 1;
  int b = 2;
  TEST_CHECK_(a + b == 3, "Expected %d, got %d", 3, a + b);



  uint32_t out;

  // Mode 0
  {
    SCT_t _cycle_data = {
      .cycle = 0,
      .iopins = iopins,
    };

    cycle_data = &_cycle_data;

    printf("\nRunning... mode 0\n");
    digitalWrite(7, HIGH);

    uint32_t input = 0xAA;
    uint32_t count = 8;
    out = spi_xfer_cpha0(input, count, iopins, timings);

    cycle_out_t out0 = printCycleData(input, count, out);
    TEST_STR(out0.sck,  "001010101010101010");
    TEST_STR(out0.mosi, "000110011001100110");
    TEST_STR(out0.miso, "111001100110011001");
  }

  // Mode 1
  {
    SCT_t _cycle_data = {
      .cycle = 0,
      .iopins = iopins,
    };

    cycle_data = &_cycle_data;

    printf("\nRunning... mode 1\n");
    digitalWrite(7, HIGH);

    uint32_t input = 0xAA;
    uint32_t count = 8;
    out = spi_xfer_cpha1(input, count, iopins, timings);

    cycle_out_t out0 = printCycleData(input, count, out);
    TEST_STR(out0.sck,  "010101010101010100");
    TEST_STR(out0.mosi, "000110011001100110");
    TEST_STR(out0.miso, "111001100110011001");
  }

  // Mode 2
  {
    SCT_t _cycle_data = {
      .cycle = 0,
      .iopins = iopins_inv,
    };

    cycle_data = &_cycle_data;

    printf("\nRunning... mode 2\n");
    digitalWrite(7, HIGH);

    uint32_t input = 0xAA;
    uint32_t count = 8;
    out = spi_xfer_cpha0(input, count, iopins_inv, timings);

    cycle_out_t out0 = printCycleData(input, count, out);
    TEST_STR(out0.sck,  "110101010101010101");
    TEST_STR(out0.mosi, "000110011001100110");
    TEST_STR(out0.miso, "111001100110011001");
  }

  // Mode 3
  {
    SCT_t _cycle_data = {
      .cycle = 0,
      .iopins = iopins_inv,
    };

    cycle_data = &_cycle_data;

    printf("\nRunning... mode 3\n");
    digitalWrite(7, HIGH);

    uint32_t input = 0xAA;
    uint32_t count = 8;
    out = spi_xfer_cpha1(input, count, iopins_inv, timings);

    cycle_out_t out0 = printCycleData(input, count, out);
    TEST_STR(out0.sck,  "110101010101010101");
    TEST_STR(out0.mosi, "000110011001100110");
    TEST_STR(out0.miso, "111001100110011001");
  }

}

TEST_LIST = {
  { "example", test_example },
  { 0 }
};

