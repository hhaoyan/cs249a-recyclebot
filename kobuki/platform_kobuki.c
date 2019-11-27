#include <stdarg.h>
#include <stdio.h>

#include <app_error.h>
#include <buckler.h>
#include <display.h>
#include <kobukiActuator.h>
#include <kobukiSensorPoll.h>
#include <kobukiUtilities.h>
#include <mpu9250.h>
#include <nrf.h>
#include <nrf_delay.h>
#include <nrf_drv_spi.h>
#include <nrf_gpio.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrf_pwr_mgmt.h>

#include "pixy.h"
#include "platform_kobuki.h"

// I2C manager
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

// LCD SPI instance
static nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);

KobukiSensors_t sensors = {0};
bool pixy_line_detected = false;
uint8_t pixy_line_start[2], pixy_line_end[2];

void init_kobuki() {
	ret_code_t error_code = NRF_SUCCESS;

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");

  // initialize LEDs
  nrf_gpio_pin_dir_set(23, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(24, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(25, NRF_GPIO_PIN_DIR_OUTPUT);

  // initialize ultrasonic sensor
  // GPIO 3: ultrasonic trig
  // GPIO 4: ultrasonic echo
  nrf_gpio_pin_dir_set(3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(4, NRF_GPIO_PIN_DIR_INPUT);
  nrf_gpio_pin_clear(3);

  // initialize display
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

  // initialize i2c master (two wire interface)
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);
  mpu9250_init(&twi_mngr_instance);
  printf("IMU initialized!\n");

  // initialize Kobuki
  kobukiInit();
  printf("Kobuki initialized!\n");
}

static char buf[16];

int lcd_printf(lcd_line_t line, const char* fmt,...) {
  va_list valist;

  memset(buf, 0, 16);

  va_start(valist, fmt);
  int ret = vsprintf(buf, fmt, valist);
  va_end(valist);

  display_write(buf, line);
  return ret;
}

void lcd_clear() {
  memset(buf, 0, 16);
  display_write(buf, 0);
  display_write(buf, 1);
}

bool is_button_press(){
  return is_button_pressed(&sensors);
}

bool is_left_cliff(){
  return sensors.cliffLeft;
}

bool is_center_cliff(){
  return sensors.cliffCenter;
}

bool is_right_cliff(){
  return sensors.cliffRight;
}

bool is_left_bumper(){
  return sensors.bumps_wheelDrops.bumpLeft;
}

bool is_right_bumper(){
  return sensors.bumps_wheelDrops.bumpRight;
}

bool is_center_bumper(){
  return sensors.bumps_wheelDrops.bumpCenter;
}

void start_gyro(){
  mpu9250_start_gyro_integration();
}

void stop_gyro(){
  mpu9250_stop_gyro_integration();
}

float read_gyro(){
  return mpu9250_read_gyro_integration().z_axis;
}

uint16_t read_encoder(){
  return sensors.leftWheelEncoder;
}

void drive_kobuki(int16_t left_wheel, int16_t right_wheel){
  kobukiDriveDirect(left_wheel, right_wheel);
}

void stop_kobuki(){
  kobukiDriveDirect(0, 0);
}

void update_sensors() {
  kobukiSensorPoll(&sensors);
  pixy_line_detected = (0 == pixy_get_line_vector(
    &pixy_line_start[0], &pixy_line_start[1], 
    &pixy_line_end[0], &pixy_line_end[1]));
}

bool is_ultrasonic_full() {
  nrf_gpio_pin_set(3);
  nrf_delay_us(15);
  nrf_gpio_pin_clear(3);
  nrf_delay_us(1300);
  return nrf_gpio_pin_read(4) > 0;
}
