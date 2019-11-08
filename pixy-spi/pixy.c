#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pixy.h"

void pixy_init() {
	spi_init();
  
  pixy_get_version();
}

void spi_recv(uint8_t* data, uint8_t len) {
	spi_trasfer_data(NULL, 0, data, len);
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

    wait_us(25);
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
  uint8_t* data = 0;
  uint8_t len = 0;
  uint8_t type = 0;

  while(pixy_packet_recv(&data, &len, &type)){
    wait_ms(5);

    // Version request
    pixy_packet_send(NULL, 0, 0x0e, 0);
    
    printf("Trying to get version...\n");
  }

  if(len != 7) {
    printf("Response size mismatch, trying to request version...\n");
  } else {
    printf("Pixy2 hardware version %d, firmware version %d:%d, build %d, firmware type \"%s\"\n",
      *((uint16_t*)(data)), *(data+2), *(data+3), *((uint16_t*)(data+4)), (char*)(data+6));
  }
  free(data);
}

void pixy_get_resolution() {
  uint8_t data = 0;
  pixy_packet_send(&data, 1, 0x0c, 0);

  uint8_t* res = 0;
  uint8_t res_len = 0;
  uint8_t res_type = 0;
  if(!pixy_packet_recv(&res, &res_len, &res_type)){
    if(res_len != 4){
      printf("Error: recv packet size != 4 for resolution!\n");
    } else {
      printf("Pixy2 resolution %dx%d\n",
        *((uint16_t*)(res)), *((uint16_t*)(res+2)));
    }
    free(res);
  } else {
    printf("Error: recv packet for resolution failed!\n");
  }
}

void pixy_set_lamp(int upper, int lower) {
  uint8_t data[2] = {upper, lower};
  pixy_packet_send(data, 2, 0x16, 0);

  uint8_t* res = 0;
  uint8_t res_len = 0;
  uint8_t res_type = 0;
  if(!pixy_packet_recv(&res, &res_len, &res_type)){
    printf("Pixy2 lamp: upper = %d, lower = %d\n", upper, lower);
    free(res);
  } else {
    printf("Error: recv packet for lamp failed!\n");
  }
}

int pixy_get_blocks(uint8_t sigmap, uint8_t max_blocks, pixy_block** blocks) {
  uint8_t data[2] = {sigmap, max_blocks};
  pixy_packet_send(data, 2, 0x20, 0);

  uint8_t* res = 0;
  uint8_t res_len = 0;
  uint8_t res_type = 0;
  if(!pixy_packet_recv(&res, &res_len, &res_type)){
    if(res_type == 0x21){
      if(res_len % sizeof(pixy_block) != 0) {
        printf("Error: recv packet size %d %% %d != 0 for obtaining block!\n", 
          res_len, sizeof(pixy_block));
      } else {
        *blocks = (pixy_block*) res;
        return res_len / sizeof(pixy_block);
      }
    } else if (res_type == 0x3) {
      printf("Error: cannot get blocks, error code: %d\n", res[0]);
    } else {
      printf("Error: cannot get blocks, response: %d\n", res_type);
    }
    
    free(res);
  } else {
    printf("Error: recv packet for obtaining blocks failed!\n");
  }
  
  return -1;
}
