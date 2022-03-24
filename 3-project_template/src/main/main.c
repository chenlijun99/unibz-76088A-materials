#include "led.h"
#include "temperature_sensor.h"

#include <stdio.h>

void app_main(void)
{
	printf("Hello World\n");
	bz_led_init();
	bz_temperature_sensor_init();
}
