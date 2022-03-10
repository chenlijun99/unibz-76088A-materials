#include "configuration.h"

#include <assert.h>
#include <stdio.h>

void configuration_load_from_nvm(struct application_config *config) {
  FILE *fp = fopen("fake_nvm", "rb");
  if (fp == NULL) {
    // assume that it is because file doesn't exist. We create one!
    fp = fopen("fake_nvm", "w+b");
    // abort if can't create file
    assert(fp != NULL);

    // set the default
#if CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT == 0
    // fake values from NVM
    config->temperature_sensor_config.use_fahrenheit = false;
#elif CONFIGURATION_FAKE_NVM_DEFAULT_USE_FAHRENHEIT == 1
    config->temperature_sensor_config.use_fahrenheit = true;
#else
    config->temperature_sensor_config.use_fahrenheit = false;
#endif

    // write the value of config to file
    size_t elements_writtent_cnt =
        fwrite(config, sizeof(struct application_config), 1, fp);
    assert(elements_writtent_cnt == 1);

    // reset cursor to beginning, so that we read from start
    rewind(fp);
  }
  size_t elements_read_cnt =
      fread(config, sizeof(struct application_config), 1, fp);
  assert(elements_read_cnt == 1);
  fclose(fp);
}
