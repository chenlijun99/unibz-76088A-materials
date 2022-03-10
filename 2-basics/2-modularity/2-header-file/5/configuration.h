#ifndef MODULARITY_EXAMPLE_CONFIGURATION_H_
#define MODULARITY_EXAMPLE_CONFIGURATION_H_

#include "temperature_sensor.h"

struct application_config {
	struct temperature_sensor_config temperature_sensor_config;
};

void configuration_init(void);
void configuration_load_from_nvm(struct application_config *config);

#endif /* ifndef MODULARITY_EXAMPLE_CONFIGURATION_H_ */
