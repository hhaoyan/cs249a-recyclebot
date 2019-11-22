#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_spi.h"

#include "buckler.h"
#include "display.h"
#include "kobukiActuator.h"
#include "kobukiSensorPoll.h"
#include "kobukiSensorTypes.h"
#include "kobukiUtilities.h"
#include "mpu9250.h"

void init_ultrasonic() {
  nrf_gpio_pin_dir_set(3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(4, NRF_GPIO_PIN_DIR_INPUT);
  nrf_gpio_pin_clear(3);
}

bool detect_ultrasonic() {
  nrf_gpio_pin_set(3);
  nrf_delay_us(15);
  nrf_gpio_pin_clear(3);
  nrf_delay_us(1500);
  return nrf_gpio_pin_read(4) > 0;
}

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  init_ultrasonic();
  nrf_delay_ms(1000);
}

