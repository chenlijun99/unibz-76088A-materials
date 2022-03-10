#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int error_number;
bool is_idle;
void init() {
}
int16_t get_current_temperature() {
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
void deinit() {
}

void foo() {
}
