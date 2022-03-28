#include "configuration.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void configuration_init(void) {
}

#ifdef CONFIGURATION_FAKE_NVM_VIA_USER_INPUT
// or alternatively #if defined(CONFIGURATION_FAKE_NVM_VIA_USER_INPUT)

void configuration_load_from_nvm(struct application_config *config) {
  printf("Please insert fake configuration for use_fahrenheit (1 for yes, 0 "
         "for no)>\n");
  int yes_or_no;
  scanf("%d", &yes_or_no);

  // Actually here we're dealing with user inputs, so using an assertion is not
  // really the right thing to do... But it's just to show you that assertions
  // exists in C.
  assert(yes_or_no >= 0 && yes_or_no <= 1);
  config->temperature_sensor_config.use_fahrenheit = yes_or_no;
}

#elif defined(CONFIGURATION_FAKE_NVM_VIA_FILE)

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

#else

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

#endif
