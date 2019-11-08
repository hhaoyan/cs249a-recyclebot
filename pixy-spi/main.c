#include <stdio.h>
#include <stdlib.h>

#include "pixy.h"

int main(void) {
  pixy_init();

  printf("Started\n");

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(0, 0);

  uint8_t* image = malloc(width * height * 3);
  while(1){
    pixy_block* blocks;
    int n_blocks = pixy_get_blocks(1, 0xff, &blocks);
    
    if(n_blocks >= 0){
      printf("Found %d blocks\n", n_blocks);
      for(int i=0;i<n_blocks;i++){
        pixy_block* block = blocks+i;
        printf("Block sig: %d, center: %d,%d; size: %dx%d, color angle: %d,"
               "traking index: %d, age: %d\n",
               block->sig, block->x, block->y, block->width, block->height, 
               block->color_angle, block->track_index, block->age);
      }
      free(blocks);
    }
    
    // printf("Read image: %d pixels\n", 
    //  pixy_get_image(&width, &height, image));
           
    wait_ms(200);
  }
}
