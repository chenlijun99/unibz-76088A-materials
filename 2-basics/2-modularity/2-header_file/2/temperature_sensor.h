#include <stdbool.h>
#include <stdint.h>

extern int temperature_sensor_error_number;

struct temperature_sensor_config {
  bool use_fahrenheit;
};

void temperature_sensor_init(const struct temperature_sensor_config *config);
int16_t temperature_sensor_get_current_temperature(void);
