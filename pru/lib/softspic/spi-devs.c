
#include <stdint.h>

#include "spi-devs.h"

#define SPI_CLOCK_MASK(value) ~(uint32_t)(value^pins.sck_dir) + 1
#define SPI_VALUE(value) ~(uint32_t)(value) + 1
#define SPI_UNPACK(value, idx) ( (value >> idx) & 0x1 )

#ifndef SPI_CUSTOM_TIMING_CALLBACK
void spi_delay(uint32_t time) { while (--time) __delay_cycles(10); }
#else
extern void spi_delay(uint32_t time);
#endif // SPI_TIMING_CALLBACK

//  clock standard
inline void spi_clock_tick(SpiPins_t pins) { digitalWrite(pins.sck, SPI_CLOCK_MASK(HIGH));}
inline void spi_clock_tock(SpiPins_t pins) { digitalWrite(pins.sck, SPI_CLOCK_MASK(LOW));}

inline uint32_t spi_reverse_bits(uint32_t value)
{
  uint32_t word = 0;
  for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
    word = (word << 1) | ((value >> i) & 0x1);
  }
  return word;
}

// ========================================================================== //
// SPI Xfer
// ========================================================================== //

/* uint8_t SpiXferTxClockRise_xfer_cycle(uint32_t value) */
inline bool spi_bit_cpha1(bool value, SpiPins_t pins, SpiTimings_t timings)
{
  bool read = 0;

  spi_clock_tick(pins);
  spi_delay(timings.c0); /* Timings::delayCyclesP0(); */
  digitalWrite(pins.mosi, SPI_VALUE(value));
  // when DataTxEdge == TxClockRise (CPOL=1) data will be captured at falling edge propagation 
  spi_delay(timings.c1); /* Timings::delayCyclesP1(); */
  spi_clock_tock(pins); /* Clock::template tock<SpiPins_t>(); */
  // holding low, so there is enough time for data preparation and changing */
  spi_delay(timings.c0); /* Timings::delayCyclesC0(); */
  read = digitalRead(pins.miso); // reading at the middle of SCK pulse
  // wait until data is fetched by slave device,  while SCK low, checking DATAsheet for this interval
  spi_delay(timings.c1); /* Timings::delayCyclesC1(); */

  return read;
}

/* uint8_t SpiXferTxClockFall_xfer_cycle(uint32_t value) */
inline bool spi_bit_cpha0(bool value, SpiPins_t pins, SpiTimings_t timings)
{
  bool read = 0;

  // changing MOSI big while SCK low, propogation
  digitalWrite(pins.mosi, SPI_VALUE(value));
  // there is a requirement that LOW and HIGH have identical interval!
  spi_delay(timings.c1); /* Timings::delayCyclesP1(); */
  spi_clock_tick(pins); /* Clock::template tick<SpiPins_t>(); */
  // reading at the middle of SCK pulse
  spi_delay(timings.c0); /* Timings::delayCyclesC0(); */
  read = digitalRead(pins.miso); // reading at the middle of SCK pulse
  // wait until data is fetched by slave device,  while SCK high, checking DATAsheet for this interval
  spi_delay(timings.c1); /* Timings::delayCyclesC1(); */
  spi_clock_tock(pins); /* Clock::template tock<SpiPins_t>(); */
  // holding low, so there is enough time for data preparation and changing
  spi_delay(timings.c0); /* Timings::delayCyclesP0(); */

  return read;
}


uint32_t spi_xfer_cpha0(uint32_t data, uint8_t bit_count, SpiPins_t pins, SpiTimings_t timings) {
  uint32_t result = 0;

  // Ensure MOSI lines low
  digitalWrite(pins.mosi, SPI_VALUE(LOW));

  // Start xfer cycle
  spi_clock_tock(pins); // Clock::template tock<SpiPins_t>();

  // Here, delay is added, to make CPHA=1 and CPHA=0 both work!
  spi_delay(timings.pre); // Timings::delayCyclesPre();

  for (uint8_t idx = 0; idx < bit_count; idx++)
    result = (result << 1) | spi_bit_cpha0(SPI_UNPACK(data, idx), pins, timings);

  digitalWrite(pins.mosi, SPI_VALUE(LOW));
  // Leave MOSI lines low

  spi_delay(timings.post); 

  return result;
}

uint32_t spi_xfer_cpha1(uint32_t data, uint8_t bit_count, SpiPins_t pins, SpiTimings_t timings) {
  uint32_t result = 0;

  // Ensure MOSI lines low
  digitalWrite(pins.mosi, SPI_VALUE(LOW));

  // Start xfer cycle
  spi_clock_tock(pins); // Clock::template tock<SpiPins_t>();

  // Here, delay is added, to make CPHA=1 and CPHA=0 both work!
  spi_delay(timings.pre); // Timings::delayCyclesPre();

  for (uint8_t idx = 0; idx < bit_count; idx++)
    result = (result << 1) | spi_bit_cpha1(SPI_UNPACK(data, idx), pins, timings);

  // Leave MOSI lines low
  digitalWrite(pins.mosi, SPI_VALUE(LOW));

  spi_delay(timings.post); 

  return result;
}

