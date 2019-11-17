#include <stdio.h>
#include <stdlib.h>

#include "pixy.h"

int main(void) {
  pixy_init();

  printf("Started\n");

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(1, 0);

  uint8_t x0, y0, x1, y1;
  // barcode* barcodes;
  while(1){
    pixy_get_line_vector(&x0, &y0, &x1, &y1);
    // get_pixy_vector();
    
    /*printf("*******Request bar codes*******\n");

    int n_codes = pixy_get_barcodes(&barcodes);

    if(n_codes > 0){
      for(int i=0;i<n_codes;i++){
        printf("Code type %d, length %d, data: %s\n", 
          barcodes[i].type, barcodes[i].len, barcodes[i].data);
        free(barcodes[i].data);
      }
      free(barcodes);
    }else if(n_codes==0){
      printf("No barcodes\n");
    }else{
      printf("Read from Pixy error!\n");
    }
    while(1);*/
    wait_ms(100);
  }
}
