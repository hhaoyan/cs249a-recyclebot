#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

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

typedef enum {
	LCD_LINE_0 = 0,
	LCD_LINE_1 = 1,
} lcd_line_t;

void init_ultrasonic() {
  nrf_gpio_pin_dir_set(3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(4, NRF_GPIO_PIN_DIR_INPUT);
  nrf_gpio_pin_clear(3);
}

bool detect_ultrasonic() {
  nrf_gpio_pin_set(3);
  nrf_delay_us(15);
  nrf_gpio_pin_clear(3);
  nrf_delay_us(1300);
  return nrf_gpio_pin_read(4) > 0;
}

int lcd_printf(lcd_line_t line, const char* fmt,...) {
  va_list valist;
  static char buf[16];

  va_start(valist, fmt);
  int ret = vsprintf(buf, fmt, valist);
  va_end(valist);

  display_write(buf, line);
  return ret;
}

// NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);
static nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  nrf_drv_spi_config_t spi_config = {
    .sck_pin = BUCKLER_LCD_SCLK,
    .mosi_pin = BUCKLER_LCD_MOSI,
    .miso_pin = BUCKLER_LCD_MISO,
    .ss_pin = BUCKLER_LCD_CS,
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0,
    .frequency = NRF_DRV_SPI_FREQ_4M,
    .mode = NRF_DRV_SPI_MODE_2,
    .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  };
  error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(error_code);
  display_init(&spi_instance);
  printf("Display initialized!\n");
  lcd_printf(LCD_LINE_0, "Hello RecycleBot!");

  init_ultrasonic();
  detect_ultrasonic();

  // while (1) {
  //   nrf_delay_ms(1000);
  // }
  while (1) {
    nrf_delay_ms(1000);
    lcd_printf(LCD_LINE_0, "%d", detect_ultrasonic());
  }
}

