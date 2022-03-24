/**
 * \file temperature_sensor.c
 * \brief Temperature sensor module - implementation
 */

#include "temperature_sensor.h"

#include <stdio.h>
#include "esp_vfs_dev.h"
#include "driver/uart.h"

void bz_temperature_sensor_init(void) {
	// See https://esp32.com/viewtopic.php?t=22929#p82561
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

	int a;
	scanf("%d", &a);
	printf("%d\n", a);
}
