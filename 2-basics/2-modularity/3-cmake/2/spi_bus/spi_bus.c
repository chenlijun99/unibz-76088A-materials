#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int spi_bus_error_number = 0;
static bool is_idle = true;
void spi_bus_init(void) {
}
void spi_bus_send(uint8_t *buffer, size_t len) {
  if (len > 0 && is_idle) {
    is_idle = false;
    printf("Sending: ");
    for (size_t i = 0; i < len; ++i) {
      printf("%x ", (unsigned int)buffer[i]);
      // or alternatively
      // printf("%x ", (unsigned int)*(buffer + i));
    }
    printf("\n");
    is_idle = true;
  }
}
void spi_bus_deinit(void) {
}
static void foo(void) {
}
