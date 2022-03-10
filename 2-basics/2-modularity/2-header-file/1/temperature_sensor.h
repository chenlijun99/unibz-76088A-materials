#include <stdint.h>

extern int temperature_sensor_error_number;

void temperature_sensor_init(void);
int16_t temperature_sensor_get_current_temperature(void);
