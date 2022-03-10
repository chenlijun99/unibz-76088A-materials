#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int error_number;
bool is_idle;
void temperature_sensor_init(void) {
}
int16_t temperature_sensor_get_current_temperature(void) {
  if (is_idle) {
    is_idle = false;
    int temperature;
    printf("Please insert the temperature> ");
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
