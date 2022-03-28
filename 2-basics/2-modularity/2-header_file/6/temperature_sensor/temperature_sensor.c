#include "temperature_sensor.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int temperature_sensor_error_number = 0;
static bool is_idle = true;
static struct temperature_sensor_config config;
void temperature_sensor_init(const struct temperature_sensor_config *a_config) {
	config = *a_config;
}
int16_t temperature_sensor_get_current_temperature(void) {
  if (is_idle) {
    is_idle = false;
    int temperature;
    printf("Please insert the temperature (%s)> ", config.use_fahrenheit ? "Fahrenheit" : "Celsius");
    scanf("%d", &temperature);
    is_idle = true;
    return temperature;
  }
  return INT16_MIN;
}
void temperature_sensor_deinit(void) {
}

static void foo(void) {
}
