#include <stdint.h>

#include <stdbool.h>
#include <stdio.h>

int error_number;

void temperature_sensor_init(void);
int16_t temperature_sensor_get_current_temperature(void);
void spi_bus_init(void);
void spi_bus_send(uint8_t *buffer, size_t len);

int main(int argc, char *argv[]) {
  spi_bus_init();
  temperature_sensor_init();

  while (1) {
    int16_t temperature = temperature_sensor_get_current_temperature();
    // How to use the error_number of temperature_sensor?
    if (error_number != 0) {
      printf("Temperature_sensor error\n");
    } else {
      spi_bus_send((uint8_t *)&temperature, sizeof(int16_t));
    }
    // How to use the error_number of spi_bus?
    if (error_number != 0) {
      printf("SPI bus error\n");
    }
  }
}
