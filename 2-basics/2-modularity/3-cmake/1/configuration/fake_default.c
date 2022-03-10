#include "configuration.h"

void configuration_load_from_nvm(struct application_config *config) {
#if CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT == 0
  // fake values from NVM
  config->temperature_sensor_config.use_fahrenheit = false;
#elif CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT == 1
  config->temperature_sensor_config.use_fahrenheit = true;
#else
  config->temperature_sensor_config.use_fahrenheit = false;
#endif
}
