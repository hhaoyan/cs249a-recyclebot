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
#include <nrf_serial.h>

#include "pixy.h"
#include "platform_kobuki.h"

// I2C manager
NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

// LCD SPI instance
static nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);

KobukiSensors_t sensors = {0};
float pixy_line_detected = 0.f;
uint8_t pixy_line_start[2], pixy_line_end[2];

static bool bin_full = false;
static int bin_full_count = 0;

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

extern const nrf_serial_t *serial_ref;
NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uart0_drv_config,
                      BUCKLER_UART_RX, BUCKLER_UART_TX,
                      0, 0,
                      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED,
                      NRF_UART_BAUDRATE_115200,
                      UART_DEFAULT_CONFIG_IRQ_PRIORITY);

void update_sensors() {
  int32_t status = kobukiSensorPoll(&sensors);

  if(status == NRF_ERROR_TIMEOUT){
    // UART stopped. Try to reinit the serial receiever.
    const nrf_serial_config_t *old_config = serial_ref->p_ctx->p_config;

    // lcd_printf(0, "Reconn Kobuki");
    nrf_serial_uninit(serial_ref);
    status = nrf_serial_init(serial_ref, &m_uart0_drv_config, old_config);
    // lcd_printf(0, "Reconn Kobuki %ld", status);
    printf("Trying to reinit serial returned %ld\n", status);
  }

  pixy_line_detected *= 0.85f;
  pixy_line_detected += (0 == pixy_get_line_vector(
    &pixy_line_start[0], &pixy_line_start[1], 
    &pixy_line_end[0], &pixy_line_end[1])) ? 0.15f : 0f;

  update_ultrasonic();
}

bool is_ultrasonic_full() {
  nrf_gpio_pin_set(3);
  nrf_delay_us(15);
  nrf_gpio_pin_clear(3);
  // change this for sensitivity
  nrf_delay_us(1100);
  return nrf_gpio_pin_read(4) > 0;
}

void update_ultrasonic() {
  // Call is_ultrasonic_full every 10 times
  static int counts = 0;
  counts++;
  counts %= 10;

  if (bin_full) return;

  if (counts) return;

  bool is_full = is_ultrasonic_full();
  if (is_full) {
    bin_full_count++;
    // only detect full when ultrasonic sensor detects full 5 more times
    if (bin_full_count > 5) {
      bin_full = true;
      bin_full_count = 0;
    }
  } else {
    bin_full_count = 0;
  }
}

bool is_bin_full() {
  return bin_full;
}

void clear_bin_full() {
  bin_full = false;
}
