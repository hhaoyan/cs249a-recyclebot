#include "app_error.h"
#include "buckler.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"

#include "pixy.h"

nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
nrf_drv_spi_config_t spi_config = {
  .sck_pin = BUCKLER_SD_SCLK,
  .mosi_pin = BUCKLER_SD_MOSI,
  .miso_pin = BUCKLER_SD_MISO,
  .ss_pin = BUCKLER_SD_CS,
  .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
  .orc = 0,
  .frequency = NRF_DRV_SPI_FREQ_2M,
  .mode = NRF_DRV_SPI_MODE_3,
  .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
};

void spi_init() {
  ret_code_t err_code;

  err_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(err_code);
}

void spi_trasfer_data(uint8_t* buf_write, uint8_t write_size, uint8_t* buf_read, uint8_t read_size) {
  ret_code_t err_code;
  err_code = nrf_drv_spi_transfer(&spi_instance, buf_write, write_size, buf_read, read_size);
  APP_ERROR_CHECK(err_code);
}

void wait_us(int us) {
  nrf_delay_us(us);
}

void wait_ms(int ms) {
  nrf_delay_ms(ms);
}

int pixy_write_image(
  const char* filename,
  uint16_t width, uint16_t height, 
  uint8_t* rgb) {
  // do nothing
  return 0;
}
