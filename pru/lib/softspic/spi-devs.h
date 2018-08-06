
#ifndef _SOFTWARE_SPI_DEVS__

#include <pru_support_lib.h>

struct SpiPins {
  uint16_t miso;
  uint16_t mosi;
  uint16_t sck;
  bool sck_dir;
};

struct SpiTimings {
  uint8_t pre;
  uint8_t c0;
  uint8_t c1;
  uint8_t post;
};

typedef struct SpiPins SpiPins_t;
typedef struct SpiTimings SpiTimings_t;

extern uint32_t spi_xfer_cpha0(uint32_t data, uint8_t bit_count, SpiPins_t pins, SpiTimings_t timings);
extern  uint32_t spi_xfer_cpha1(uint32_t data, uint8_t bit_count, SpiPins_t pins, SpiTimings_t timings);

// override definition by defining `SPI_CUSTOM_TIMING_CALLBACK`
extern void spi_delay(uint32_t time);


#endif // _SOFTWARE_SPI_DEVS__
