#include "configuration.h"

#include <assert.h>
#include <stdio.h>

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
