#ifndef MODULARITY_EXAMPLE_SPI_BUS_H_
#define MODULARITY_EXAMPLE_SPI_BUS_H_

#include <stdint.h>
#include <stdlib.h>

extern int spi_bus_error_number;

void spi_bus_init(void);
void spi_bus_send(uint8_t *buffer, size_t len);

#endif /* ifndef MODULARITY_EXAMPLE_SPI_BUS_H_ */
