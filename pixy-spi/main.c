#include <stdio.h>

#include "buckler.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "app_error.h"

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

void pixy_init() {
	ret_code_t err_code;

	err_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
	APP_ERROR_CHECK(err_code);
}

void pixy_trasfer_data(uint8_t* buf_write, uint8_t write_size, uint8_t* buf_read, uint8_t read_size) {
	ret_code_t err_code;
	err_code = nrf_drv_spi_transfer(&spi_instance, buf_write, write_size, buf_read, read_size);
	APP_ERROR_CHECK(err_code);
}

void pixy_recv(uint8_t* data, uint8_t len) {
	pixy_trasfer_data(NULL, 0, data, len);
}

void pixy_send(uint8_t* data, uint8_t len) {
	pixy_trasfer_data(data, len, NULL, 0);
}

int main(void) {
  pixy_init();

  uint8_t version_request[] = {0xae, 0xc1, 0x0e, 0x00};
  pixy_send(version_request, 4);

  // Clear out stale data
  int q=1;

  while(1){
    uint8_t data[22];
    memset(data, 0, 22);

  for(int i=0;i<100;i++){
    pixy_recv(data, 22);
  }

    pixy_trasfer_data(version_request, 4, data, 4);
    for(int i=0;i<4;i++){
      printf("0x%x ", data[i]);
    }
    nrf_delay_ms(300);
    pixy_recv(data, 22);
    // pixy_trasfer_data(version_request, q?4:0, data, 22);
    // q = 1-q;
    // pixy_send(version_request, 4);
    // printf("Start: 0x%x 0x%x\n", data[0], data[1]);
    // pixy_recv(data, 22);
  	// pixy_recv(&data, 10);
    for(int i=0;i<22;i++){
      printf("0x%x ", data[i]);
    }
    printf("\n");
  	// printf("0x%x 0x%x\n", data[0], data[1]);
  	nrf_delay_ms(100);
  }
  while(true){
  	;
  }
}

