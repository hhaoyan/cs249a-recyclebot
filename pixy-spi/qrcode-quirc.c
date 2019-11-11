#include <quirc.h>
#include <stdio.h>

#include "pixy.h"

struct quirc *qr = 0;

int pixy_decode_qr(
  uint16_t width, uint16_t height,
  uint8_t* rgb) {
    
  if(!qr){
    qr = quirc_new();
    if(!qr){
      printf("Error: cannot allocate quirc struct!\n");
      return -1;
    }
    
    if(quirc_resize(qr, width, height) < 0){
      printf("Error: cannot allocate quirc buffer!\n");
      return -1;
    }
  }

  int w, h;
  uint8_t *gray_scale = quirc_begin(qr, &w, &h);
  uint8_t max_gray = 0;

  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++){
      uint8_t r = rgb[(i*width+j)*3+0],
              g = rgb[(i*width+j)*3+1],
              b = rgb[(i*width+j)*3+2];
      uint8_t gray = (uint8_t)(r*2126/10000 + g*7152/10000 + b*722/10000);
      gray_scale[i*width+j] = gray;
      if(max_gray < gray)
        max_gray = gray;
    }
  }
  for(int i=0;i<height*width;i++){
    uint16_t corrected = gray_scale[i] * 255/max_gray;
    gray_scale[i] = (uint8_t) corrected;
  }
  
  quirc_end(qr);

  int num_codes = quirc_count(qr);
  for (int i = 0; i < num_codes; i++) {
    struct quirc_code code;
    struct quirc_data data;
    quirc_decode_error_t err;

    quirc_extract(qr, i, &code);

    /* Decoding stage */
    err = quirc_decode(&code, &data);
    if (err)
        printf("DECODE FAILED: %s\n", quirc_strerror(err));
    else
        printf("Data: %s\n", data.payload);
  }

  return num_codes;
}
