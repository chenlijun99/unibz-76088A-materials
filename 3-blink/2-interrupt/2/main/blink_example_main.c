/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include <stdio.h>

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to
   blink, or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

/*
 * 0 => LED on
 * 1 => LED off
 */
static uint8_t s_led_state = 0;

static led_strip_t *pStrip_a;

static void blink_led(void) {
  /* If the addressable LED is enabled */
  if (s_led_state) {
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
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

#define BUTTON_GPIO 9
#define BUTTON_INTERRUPT_FLAG 0

static SemaphoreHandle_t button_semaphore = NULL;

static void gpio_isr_handler(void *arg) {
  BaseType_t higher_priority_task_woken = pdFALSE;
  xSemaphoreGiveFromISR(button_semaphore, &higher_priority_task_woken);
  if (higher_priority_task_woken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

static void bz_gpio_init() {
  // Configure a interrupt that is triggered whenever we click the button: i.e.
  // press the button and then release it.

  gpio_config_t io_conf;
  // interrupt of rising edge (i.e. when we release the button)
  io_conf.intr_type = GPIO_INTR_POSEDGE;
  // set as input mode
  io_conf.mode = GPIO_MODE_INPUT;
  // bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = 1 << BUTTON_GPIO;
  // disable pull-down mode
  io_conf.pull_down_en = 0;
  // enable pull-up mode
  io_conf.pull_up_en = 1;
  // configure GPIO with the given settings
  gpio_config(&io_conf);

  gpio_install_isr_service(BUTTON_INTERRUPT_FLAG);
  // hook isr handler for specific gpio pin
  gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL);
}

void app_main(void) {
  // allocate and initialize binary semaphore.
  button_semaphore = xSemaphoreCreateBinary();

  bz_gpio_init();

  /* Configure the peripheral according to the LED type */
  configure_led();

  bool button_toggle_status = false;
  while (1) {
    // Wait for button interrupt to unblock us, with a timeout of
    // CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS, after which xSemaphoreTake
    // returns pdFALSE (i.e. false), which means that the semaphore was not
    // taken.
    // We use this timeout to blink the LED, when no button press happens.
    if (xSemaphoreTake(button_semaphore,
                       CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS)) {
      button_toggle_status = !button_toggle_status;
    }

    if (button_toggle_status || !s_led_state) {
      ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
      blink_led();
      /* Toggle the LED state */
      s_led_state = !s_led_state;
    }
  }
}
