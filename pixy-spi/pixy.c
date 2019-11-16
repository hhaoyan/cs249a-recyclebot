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

  if(len != 16) {
    printf("Response size mismatch, expected %d got %d, data bytes:", 7, len);
    for(int i=0;i<len;i++){
      printf("0x%02x ", data[i]);
    }
    printf("\n");
  } else {
    printf("Pixy2 hardware version %d, firmware version %d:%d, build %d, firmware type \"%s\"\n",
      *((uint16_t*)(data)), *(data+2), *(data+3), *((uint16_t*)(data+4)), (char*)(data+6));
  }
  free(data);
}

void pixy_get_resolution(uint16_t* width, uint16_t* height) {
  uint8_t data = 0;
  pixy_packet_send(&data, 1, 0x0c, 0);

  uint8_t* res = 0;
  uint8_t res_len = 0;
  uint8_t res_type = 0;
  if(!pixy_packet_recv(&res, &res_len, &res_type)){
    if(res_len != 4){
      printf("Error: recv packet size != 4 for resolution!\n");
    } else {
      *width = *((uint16_t*)(res));
      *height = *((uint16_t*)(res+2));
      printf("Pixy2 resolution %dx%d\n", *width, *height);
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

int pixy_get_line_features(
  line_feature_req req_group, line_feature_req req_type, line_feature** features) {
  uint8_t data[2] = {req_group, req_type};
  pixy_packet_send(data, 2, 0x30, 0);

  uint8_t* res = 0;
  uint8_t res_len = 0;
  uint8_t res_type = 0;
  if(!pixy_packet_recv(&res, &res_len, &res_type)){
    if(res_type == 0x31){

      uint8_t n_features = 0;
      for(int i=0;i<res_len;){
        n_features += 1;
        i += (2+res[i+1]);
      }
      *features = malloc(sizeof(line_feature)*n_features);

      uint8_t *pt = res;
      for(int i=0;i<n_features;i++){
        (*features + i)->type = pt[0];
        (*features + i)->len = pt[1];
        (*features + i)->data = malloc(pt[1]);
        memcpy((*features + i)->data, pt+2, pt[1]);
        pt += 2 + pt[1];
      }
      free(res);
      return n_features;
    } else if (res_type == 0x3) {
      printf("Error: cannot get line features, error code: %d\n", res[0]);
    } else {
      printf("Error: cannot get line features, response: %d\n", res_type);
    }
    
    free(res);
  } else {
    printf("Error: recv packet for obtaining line features failed!\n");
  }
  
  return -1;
}

// Read packet without memory allocation, make sure buf is large enough!
int _read_package_fast(uint8_t* buf, uint8_t* len, uint8_t* type) {
  uint16_t sync = wait_for_sync();

  if (sync == 0xc1ae) {
    uint8_t header[2];
    spi_recv(header, 2);

    *type = header[0];
    *len = header[1];
    spi_recv(buf, *len);
    
    return 0;
  } else if (sync == 0xc1af) {
    // Need to perform checksum
    uint8_t header[4];
    spi_recv(header, 4);

    *type = header[0];
    *len = header[1];
    spi_recv(buf, *len);

    uint16_t checksum = 0;
    for(int i=0;i<*len;i++) {
      checksum += buf[i];
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

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

int pixy_get_image(
  uint16_t width, uint16_t height, 
  uint8_t* rgb) {
    uint8_t len, type;
    
    uint8_t data[5];
    uint32_t * start_addr = (uint32_t*) data;
    uint8_t * req_len = (uint8_t*)(data+4);
    uint32_t recvd = 0;
    uint8_t * bayer_buffer = malloc(width*height);
    
    while(recvd < width * height){
      *req_len = min(width * height - recvd, 249);
      *start_addr = recvd;
      //printf("req: %d->%d bytes, %d recvd\n", 
      //  *start_addr, (*req_len) + (*start_addr), recvd);
      
      pixy_packet_send(data, 5, 0x75, 0);
      
      if(_read_package_fast(bayer_buffer+recvd, &len, &type)){
          printf("Error: read image, early termination\n");
          return recvd;
      }
      if(type != 0x76 || len != (*req_len)){
        printf("Error: read image response, packet type %d, length %d\n", 
          type, len);
        return recvd;
      }
      
      recvd += (*req_len);
    }
    
    printf("Recvd %d bytes image\n", recvd);
    
    for(int i=0;i<height;i++){
      for(int j=0;j<width;j++){
        uint8_t r, g, b;
        uint8_t* pixel = bayer_buffer + i*width + j;
        if(i&1){
          if(j&1){
            r = *pixel;
            g = (*(pixel-1) + *(pixel+1) + *(pixel+width) + *(pixel-width)) / 4;
            b = (*(pixel-width-1) + *(pixel-width+1) + *(pixel+width-1) + *(pixel+width+1)) / 4;
          } else {
            r = (*(pixel-1) + *(pixel+1))/2;
            g = *pixel;
            b = (*(pixel-width) + *(pixel+width)) / 2;
          }
        } else {
          if(j&1){
            r = (*(pixel-width) + *(pixel+width))/2;
            g = *pixel;
            b = (*(pixel-1) + *(pixel+1)) / 2;
          } else {
            r = (*(pixel-width-1) + *(pixel-width+1) + *(pixel+width-1) + *(pixel+width+1)) / 4;
            g = (*(pixel-1) + *(pixel+1) + *(pixel+width) + *(pixel-width)) / 4;
            b = *pixel;
          }
        }
        
        rgb[(i*width + j)*3 + 0] = r;
        rgb[(i*width + j)*3 + 1] = g;
        rgb[(i*width + j)*3 + 2] = b;
      }
    }
    
    free(bayer_buffer);
    
    return recvd;
}

int pixy_get_barcodes(barcode** codes) {
  pixy_packet_send(NULL, 0, 0x77, 0);

  uint8_t* res = 0;
  uint8_t res_len = 0;
  uint8_t res_type = 0;
  if(!pixy_packet_recv(&res, &res_len, &res_type)){
    if(res_type == 0x78){

      uint8_t n_codes = 0;
      for(int i=0;i<res_len;){
        n_codes += 1;
        i += (2+res[i+1]);
      }
      *codes = malloc(sizeof(barcode)*n_codes);

      uint8_t *pt = res;
      for(int i=0;i<n_codes;i++){
        (*codes + i)->type = pt[0];
        (*codes + i)->len = pt[1];
        (*codes + i)->data = malloc(pt[1]);
        memcpy((*codes + i)->data, pt+2, pt[1]);
        pt += 2 + pt[1];
      }
      free(res);
      return n_codes;
    } else if (res_type == 0x3) {
      printf("Error: cannot get barcodes, error code: %d\n", res[0]);
    } else {
      printf("Error: cannot get barcodes, response: %d\n", res_type);
    }
    
    free(res);
  } else {
    printf("Error: recv packet for obtaining barcodes failed!\n");
  }
  
  return -1;
}
