#include <stdio.h>
#include <stdlib.h>

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

void spi_trasfer_data(uint8_t* buf_write, uint8_t write_size, uint8_t* buf_read, uint8_t read_size) {
	ret_code_t err_code;
	err_code = nrf_drv_spi_transfer(&spi_instance, buf_write, write_size, buf_read, read_size);
	APP_ERROR_CHECK(err_code);
}

void spi_recv(uint8_t* data, uint8_t len) {
  // static uint8_t buf[1] = {0};
	spi_trasfer_data(NULL, 0, data, len);
  // spi_trasfer_data(buf, 1, data, len);
}

void spi_send(uint8_t* data, uint8_t len) {
	spi_trasfer_data(data, len, NULL, 0);
}

/**
  * Send a packet.
  */
void pixy_packet_send(uint8_t* data, uint8_t len, uint8_t packet_type, int do_checksum) {
  uint16_t checksum = 0;
  if(do_checksum) {
    for(int i=0;i<len;i++){
      checksum += data[i];
    }

    uint8_t header[6] = {
      0xaf, 0xc1, // no_checksum_sync 0xc1af
      packet_type, // packet type
      len, // length of data
      checksum & 0xff, (checksum >> 8)&0xff, // checksum
    };
    spi_send(header, 6);
    spi_send(data, len);
  } else {
    uint8_t header[4] = {
      0xae, 0xc1, // no_checksum_sync 0xc1ae
      packet_type, // packet type
      len, // length of data
    };
    spi_send(header, 4);
    spi_send(data, len);
  }
}

uint16_t wait_for_sync() {
  uint8_t retry = 0;
  uint8_t data[2];
  uint16_t* sync = (uint16_t*)data;

  spi_recv(data, 1);

  // Pixy2 specs declare that pixy2 data must return in 100ms.
  while(retry < 5) {
    for(int i=0;i<4;i++){
      spi_recv(data+1, 1);
      if (*sync == 0xc1ae || *sync == 0xc1af) {
        return *sync;
      }
      data[0] = data[1];
    }

    nrf_delay_us(25);
    retry++;
  }

  return -1;
}

/**
  * Recv a packet, data will be a chunk of memory allocated by malloc and must be freed
  * by the calling function.
  */
int pixy_packet_recv(uint8_t** data, uint8_t* len, uint8_t* packet_type) {
  uint16_t sync = wait_for_sync();

  if (sync == 0xc1ae) {
    uint8_t header[2];
    spi_recv(header, 2);

    *packet_type = header[0];
    *len = header[1];

    *data = malloc(*len);
    spi_recv(*data, *len);
    return 0;
  } else if (sync == 0xc1af) {
    // Need to perform checksum
    uint8_t header[4];
    spi_recv(header, 4);

    *packet_type = header[0];
    *len = header[1];

    *data = malloc(*len);
    spi_recv(*data, *len);

    uint16_t checksum = 0;
    for(int i=0;i<*len;i++) {
      checksum += (*data)[i];
    }

    uint16_t* checksum_recv = (uint16_t*)(header+2);
    if(checksum != *checksum_recv){
      printf("Checksum mismatch, expected 0x%x, got 0x%x\n", 
        *checksum_recv, checksum);
      return 1;
    }

    return 0;
  }

  return 1;
}

void pixy_get_version() {
  uint8_t* data;
  uint8_t len;
  uint8_t type;

  while(pixy_packet_recv(&data, &len, &type)){
    nrf_delay_ms(5);

    // Version request
    pixy_packet_send(NULL, 0, 0x0e, 0);
    printf("Trying to get version...\n");
  }

  printf("Pixy2 hardware version %d, firmware version %d:%d, build %d, firmware type \"%s\"\n",
    *((uint16_t*)(data)), *(data+2), *(data+3), *((uint16_t*)(data+4)), (char*)(data+6));
  free(data);
}

void pixy_get_resolution() {
  uint8_t data = 0;
  pixy_packet_send(&data, 1, 0x0c, 0);

  uint8_t* res;
  uint8_t res_len;
  uint8_t res_type;
  APP_ERROR_CHECK(pixy_packet_recv(&res, &res_len, &res_type));

  printf("Pixy2 resolution %dx%d\n",
    *((uint16_t*)(res)), *((uint16_t*)(res+2)));
  free(res);
}

void pixy_set_lamp(int upper, int lower) {
  uint8_t data[2] = {upper, lower};
  pixy_packet_send(data, 2, 0x16, 0);

  uint8_t* res;
  uint8_t res_len;
  uint8_t res_type;
  APP_ERROR_CHECK(pixy_packet_recv(&res, &res_len, &res_type));

  free(res);
}

int main(void) {
  pixy_init();

  printf("Started\n");

  pixy_get_version();
  pixy_get_resolution();
  pixy_set_lamp(0, 0);

  while(true){

  	;
  }
}

