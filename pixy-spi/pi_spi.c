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
