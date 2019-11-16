#include <stdio.h>
#include <stdlib.h>

#include "pixy.h"

int main(void) {
  pixy_init();

  printf("Started\n");

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(1, 0);

  line_feature* features;
  barcode* barcodes;
  while(1){
    /*
    printf("*******Request line features*******\n");

    int n_features = pixy_get_line_features(
      LINE_FEA_REQ_MAIN, 
      LINE_FEA_VECTOR|LINE_FEA_INTERSECTION|LINE_FEA_BARCODE,
      &features);

    if(n_features > 0){
      for(int i=0;i<n_features;i++){
        switch(features[i].type){
          case LINE_FEA_VECTOR:
          {
            for(int j = 0;j<features[i].len;j+=6){
              uint8_t *data = ((uint8_t*)features[i].data) + j;
              printf("Vector: (%d,%d) -> (%d,%d), index: %d, flags: %d\n",
                data[0], data[1], data[2], data[3], data[4], data[5]);
            }
          }
          break;

          case LINE_FEA_INTERSECTION: 
          {}
          break;

          case LINE_FEA_BARCODE:
          {
            for(int j = 0;j<features[i].len;j+=4){
              uint8_t *data = ((uint8_t*)features[i].data) + j;
              printf("Barcode: (%d,%d), code: %d, flags: %d\n",
                data[0], data[1], data[3], data[2]);
            }
          }
          break;

          default:
          break;
        }
        free(features[i].data);
      }
      free(features);
    }else if(n_features==0){
      printf("No line features\n");
    }else{
      printf("Read from Pixy error!\n");
    }*/
    
    printf("*******Request bar codes*******\n");

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
    wait_ms(1000);
  }
}
