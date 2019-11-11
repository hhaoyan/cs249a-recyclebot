#include <stdio.h>
#include <stdlib.h>
#include <zbar.h>

#include "pixy.h"

zbar_image_scanner_t *scanner = 0;

int pixy_decode_qr(
  uint16_t width, uint16_t height,
  uint8_t* rgb) {
    
  if(!scanner){
    scanner = zbar_image_scanner_create();

    if(!scanner){
      printf("Error: cannot allocate zbar instance!\n");
      return -1;
    }
    
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);
  }

  int w, h;
  uint8_t *gray_scale = malloc(width*height);
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

  zbar_image_t *image = zbar_image_create();
  zbar_image_set_format(image, *(int*)"Y800");
  zbar_image_set_size(image, width, height);
  zbar_image_set_data(image, gray_scale, width * height, zbar_image_free_data);
  
  int num_codes = zbar_scan_image(scanner, image);

  const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
  for(; symbol; symbol = zbar_symbol_next(symbol)) {
      zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
      const char *data = zbar_symbol_get_data(symbol);
      printf("decoded %s symbol \"%s\"\n",
             zbar_get_symbol_name(typ), data);
  }

  zbar_image_destroy(image);

  return num_codes;
}
