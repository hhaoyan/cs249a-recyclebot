#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPiSPI.h>
#include <unistd.h>

#include "pixy.h"

void spi_init() {
  if(wiringPiSPISetup(0, 2000000) < 0){
    printf("Unable to open device\n");
    exit(1);
  }
}

void spi_trasfer_data(
  uint8_t* buf_write, uint8_t write_size, 
  uint8_t* buf_read, uint8_t read_size) {  
  uint8_t* buf = 0;
  uint8_t bufsize;
  
  if(buf_write){
    buf = malloc(write_size);
    bufsize = write_size;
    memcpy(buf, buf_write, write_size);
  }else if (buf_read){
    buf = malloc(read_size);
    bufsize = read_size;
    memset(buf, 0, read_size);
  }
  
  if(buf){
    if(wiringPiSPIDataRW(0, buf, bufsize) < 0){
      printf("Unable to do R/W\n");
      exit(1);
    }    
  }
  
  if(buf_read){
    memcpy(buf_read, buf, read_size);
  }
  
  if(buf)
    free(buf);
}

void wait_us(int us) {
  usleep(us);
}

void wait_ms(int ms) {
  usleep(ms*1000);
}

int pixy_write_image(
  const char* filename,
  uint16_t width, uint16_t height, 
  uint8_t* rgb) {
  uint8_t bmp_header[] = {
    0x42, 0x4d, // BM
    0x78, 0x02, 0x04, 0x00, // 197240 Bytes, 54 header + image + 2 padding
    0x00, 0x00, 0x00, 0x00, // Reserved
    0x36, 0x00, 0x00, 0x00, // To bitmap data
    0x28, 0x00, 0x00, 0x00, // BMP header 40 bytes
    0x3c, 0x01, 0x00, 0x00, // Width 316
    0xd0, 0x00, 0x00, 0x00, // Height 208
    0x01, 0x00, 0x18, 0x00, // One plane, 24 bit
    0x00, 0x00, 0x00, 0x00, // No compression
    0x42, 0x02, 0x03, 0x00, // Size of image data (316*208*3 padded)
    0x12, 0x0b, 0x00, 0x00, // Resolution x
    0x12, 0x0b, 0x00, 0x00, // Resolution y
    0x00, 0x00, 0x00, 0x00, // Number of colors
    0x00, 0x00, 0x00, 0x00, // Important colors
  };
  uint8_t padding[] = {0x00, 0x00, 0x00, 0x00};
  
  uint32_t rgb_size = width * height * 3;
  uint32_t padding_size = (((rgb_size + 54) >> 2) << 2) - 54;
  *(uint32_t*)(bmp_header + 2) = (54 + rgb_size + padding_size);
  *(uint32_t*)(bmp_header + 18) = width;
  *(uint32_t*)(bmp_header + 22) = height;
  *(uint32_t*)(bmp_header + 34) = rgb_size + padding_size;
  
  FILE * file = fopen(filename, "wb");
  fwrite(bmp_header, sizeof(bmp_header), 1, file);
  for(int i=0;i<height;i++){
    fwrite(rgb+(height-i-1)*width*3, width*3, 1, file);
  }
  fwrite(padding, padding_size, 1, file);
  fclose(file);
}
