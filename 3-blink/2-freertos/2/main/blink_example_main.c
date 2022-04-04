/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include <stdio.h>

#include "driver/uart.h"
#include "esp_vfs_dev.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to
   blink, or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t s_led_state = 0;

static led_strip_t *pStrip_a;

static void blink_led(int red, int green, int blue) {
  /* If the addressable LED is enabled */
  if (s_led_state) {
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    pStrip_a->set_pixel(pStrip_a, 0, red, green, blue);
    /* Refresh the strip to send data */
    pStrip_a->refresh(pStrip_a, 100);
  } else {
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
  }
}

static void configure_led(void) {
  ESP_LOGI(TAG, "Example configured to blink addressable LED!");
  /* LED strip initialization with the GPIO and pixels number*/
  pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
  /* Set all LED off to clear all pixels */
  pStrip_a->clear(pStrip_a, 50);
}

// We used this struct to communicate between the two tasks.
// We communicate by sharing memory.
static struct {
  int red;
  int green;
  int blue;
} g_task_shared = {
    // Use these RGB values as default. I picked them from UniBZ's logo.
    .red = 9,
    .green = 115,
    .blue = 186,
};

static void led_blink_task(void *params) {
  /* Configure the peripheral according to the LED type */
  configure_led();

  while (1) {
    ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
    blink_led(g_task_shared.red, g_task_shared.green, g_task_shared.blue);
    /* Toggle the LED state */
    s_led_state = !s_led_state;
    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
  }
}

static void input_task(void *params) {
  // To make scanf work
  // See https://esp32.com/viewtopic.php?t=22929#p82561
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  ESP_ERROR_CHECK(
      uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
  esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CR);
  esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM,
                                            ESP_LINE_ENDINGS_CRLF);
  while (1) {
    printf("Insert red intensity\n");
    scanf("%d", &g_task_shared.red);
    printf("Insert green intensity\n");
    scanf("%d", &g_task_shared.green);
    printf("Insert blue intensity\n");
    scanf("%d", &g_task_shared.blue);
    printf("RGB: (%d, %d, %d)\n", g_task_shared.red, g_task_shared.green,
           g_task_shared.blue);
  }
}

void app_main(void) {
  xTaskCreate(led_blink_task, NULL, 0x1024, NULL, tskIDLE_PRIORITY + 1, NULL);
  // Try to change 0x1024 to 0x512 and then build and flash. You'll witness a
  // stack overflow!
  xTaskCreate(input_task, NULL, 0x1024, NULL, tskIDLE_PRIORITY + 1, NULL);
}
