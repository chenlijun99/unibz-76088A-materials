#include <stdint.h>

#include <stdbool.h>
#include <stdio.h>

int error_number;

int main(int argc, char *argv[]) {
  // How should I invoke the init of temperature_sensor and the init of spi_bus?
  init();
  init();

  while (1) {
    int16_t temperature = get_current_temperature();
    // How to use the error_number of temperature_sensor?
    if (error_number != 0) {
      printf("Temperature_sensor error\n");
    } else {
      send((uint8_t *)&temperature, sizeof(int16_t));
    }
    // How to use the error_number of spi_bus?
    if (error_number != 0) {
      printf("SPI bus error\n");
    }
  }
}
