#include "configuration.h"

void configuration_init(void) {
}
void configuration_load_from_nvm(struct application_config *config) {
  // fake values from NVM
  config->temperature_sensor_config.use_fahrenheit = false;
}
