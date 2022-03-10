#include "spi_bus.h"
#include "temperature_sensor.h"

#include <stdint.h>

#include <stdbool.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  spi_bus_init();
  temperature_sensor_init();

  while (1) {
    int16_t temperature = temperature_sensor_get_current_temperature();
    if (temperature_sensor_error_number != 0) {
      printf("Temperature_sensor error\n");
    } else {
      spi_bus_send((uint8_t *)&temperature, sizeof(int16_t));
    }
    if (spi_bus_error_number != 0) {
      printf("SPI bus error\n");
    }
  }
}
